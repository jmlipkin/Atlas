#include "atpch.h"
#include "Renderer.h"

#include "Atlas/Core/Log.h"
#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Font.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"
#include "Atlas/Renderer/Tileset.h"

#include "Atlas/Project/Project.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Atlas {

struct Vertex {
	glm::vec3 position;
	glm::vec4 color;
	uint32_t  texIndex;
	glm::vec2 texCoord;
};

struct RendererData {
	const uint32_t maxVertexCount = 50000;
	const uint32_t maxIndexCount  = 100000;

	static const uint32_t maxTextureSlots = 32;

	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer>  indexBuffer;

	std::shared_ptr<Pipeline>	   polygonPipeline;
	std::shared_ptr<UniformBuffer> polygonUniforms;

	std::shared_ptr<Pipeline>	   textPipeline;
	std::shared_ptr<UniformBuffer> textUniforms;

	std::array<std::shared_ptr<Texture>, maxTextureSlots> textureSlots;

	uint32_t textureSlotIndex = 1;	// 0 = white texture, 1 = invalid texture

	uint32_t indexCount		  = 0;
	uint32_t vertexCount	  = 0;
	Vertex*	 vertexBufferBase = nullptr;
	Vertex*	 vertexPtr		  = nullptr;
	Vertex*	 batchStartPtr	  = nullptr;

	uint32_t* indexBufferBase = nullptr;
	uint32_t* indexPtr;

	std::shared_ptr<Pipeline>	   currentPipeline		= nullptr;
	std::shared_ptr<UniformBuffer> currentUniformBuffer = nullptr;

	glm::vec4 vertexPositions[4];
};

static RendererData s_data;
static float		s_pixelsPerWorldUnit;

///////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::switchPipeline(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<UniformBuffer> uniforms) {
	if (s_data.currentPipeline != pipeline) {
		flush();
		s_data.currentPipeline		= pipeline;
		s_data.currentUniformBuffer = uniforms;
	}
}

// Looks up or registers a texture in the current batch's texture slots.
// Returns the slot index for use in vertex data.
uint32_t Renderer::getTextureIndex(const std::shared_ptr<Texture>& texture) {
	for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
		if (*texture == *s_data.textureSlots[i])
			return i;
	}

	if (s_data.textureSlotIndex >= RendererData::maxTextureSlots)
		flush();

	uint32_t index			   = s_data.textureSlotIndex;
	s_data.textureSlots[index] = texture;
	s_data.textureSlotIndex++;
	return index;
}

// Writes 4 vertices and increments the index count for one quad.
void Renderer::submitQuad(const glm::mat4& transform,
						  const glm::vec4& color,
						  uint32_t		   texIndex,
						  const glm::vec2  texCoords[4]) {
	if (s_data.indexCount + 6 > s_data.maxIndexCount) {
		flush();
	}

	uint32_t baseVertex = s_data.vertexCount;

	for (size_t i = 0; i < 4; i++) {
		s_data.vertexPtr->position = transform * s_data.vertexPositions[i];
		s_data.vertexPtr->color	   = color;
		s_data.vertexPtr->texIndex = texIndex;
		s_data.vertexPtr->texCoord = texCoords[i];
		s_data.vertexPtr++;
	}

	s_data.indexPtr[0] = baseVertex + 0;
	s_data.indexPtr[1] = baseVertex + 1;
	s_data.indexPtr[2] = baseVertex + 2;
	s_data.indexPtr[3] = baseVertex + 2;
	s_data.indexPtr[4] = baseVertex + 3;
	s_data.indexPtr[5] = baseVertex + 0;

	s_data.indexCount += 6;
	s_data.indexPtr += 6;
	s_data.vertexCount += 4;
}

///////////////////////////////////////////////////////////////////////////////
//
// Lifecycle
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::init(GraphicsContext& context) {
	AT_PROFILE_FUNCTION();

	RenderCommand::init(context);

	AssetManager::loadShader("Polygon Shader", "polygonVertexShader", "polygonFragmentShader");
	AssetManager::loadShader("Text Shader", "textVertexShader", "textFragmentShader");

	s_data.vertexBufferBase = new Vertex[s_data.maxVertexCount];
	s_data.vertexBuffer		= VertexBuffer::create(s_data.maxVertexCount * sizeof(Vertex));

	s_data.indexBufferBase = new uint32_t[s_data.maxIndexCount];
	s_data.indexBuffer	   = IndexBuffer::create(s_data.indexBufferBase, s_data.maxIndexCount);

	// Quad pipeline
	PipelineSpecification polygonPipelineSpecs;
	polygonPipelineSpecs.name	= "Polygon Pipeline";
	polygonPipelineSpecs.shader = AssetManager::get<Shader>("Polygon Shader");
	polygonPipelineSpecs.layout = BufferLayout({{"a_position", ShaderDataType::Float3},
												{"a_color", ShaderDataType::Float4},
												{"a_texIndex", ShaderDataType::Uint},
												{"a_texCoord", ShaderDataType::Float2}});

	s_data.polygonUniforms = UniformBuffer::create(
		polygonPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
		1);

	s_data.polygonPipeline = Pipeline::create(polygonPipelineSpecs);

	// Text pipeline
	PipelineSpecification textPipelineSpecs;
	textPipelineSpecs.name			   = "Text Pipeline";
	textPipelineSpecs.shader		   = AssetManager::get<Shader>("Text Shader");
	textPipelineSpecs.enableDepthWrite = false;
	textPipelineSpecs.layout		   = BufferLayout({{"a_position", ShaderDataType::Float3},
													   {"a_color", ShaderDataType::Float4},
													   {"a_texIndex", ShaderDataType::Uint},
													   {"a_texCoord", ShaderDataType::Float2}});

	s_data.textUniforms = UniformBuffer::create(
		textPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
		2);
	s_data.textPipeline = Pipeline::create(textPipelineSpecs);

	// Texture slot 0 is reserved for 1x1 white texture
	std::shared_ptr<Texture> whiteTexture	  = Texture::create(1, 1);
	uint32_t				 whiteTextureData = 0xFFFFFFFF;
	whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));
	s_data.textureSlots[0] = whiteTexture;

	s_data.vertexPositions[0] = {0.0f, 1.0f, 0.0f, 1.0f};
	s_data.vertexPositions[1] = {1.0f, 1.0f, 0.0f, 1.0f};
	s_data.vertexPositions[2] = {1.0f, 0.0f, 0.0f, 1.0f};
	s_data.vertexPositions[3] = {0.0f, 0.0f, 0.0f, 1.0f};

	AT_CORE_TRACE("Renderer initialized");
}

void Renderer::shutdown() {
	AT_PROFILE_FUNCTION();

	delete[] s_data.vertexBufferBase;
	delete[] s_data.indexBufferBase;
}

void Renderer::beginScene(const OrthographicCamera& camera) {
	AT_PROFILE_FUNCTION();

	s_data.polygonUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
	s_data.textUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

	s_data.vertexPtr	 = s_data.vertexBufferBase;
	s_data.batchStartPtr = s_data.vertexBufferBase;

	s_data.indexPtr = s_data.indexBufferBase;

	s_pixelsPerWorldUnit = camera.getPixelsPerWorldUnit();

	startNewBatch();
}

void Renderer::endScene() {
	AT_PROFILE_FUNCTION();

	flush();
}

void Renderer::startNewBatch() {
	AT_PROFILE_FUNCTION();

	s_data.vertexCount		= 0;
	s_data.indexCount		= 0;
	s_data.batchStartPtr	= s_data.vertexPtr;
	s_data.indexPtr			= s_data.indexBufferBase;
	s_data.textureSlotIndex = 1;

	s_data.currentPipeline		= nullptr;
	s_data.currentUniformBuffer = nullptr;
}

void Renderer::flush() {
	AT_PROFILE_FUNCTION();

	if (s_data.indexCount == 0 || !s_data.currentPipeline || !s_data.currentUniformBuffer) {
		startNewBatch();
		return;
	}

	// Calculate offset of current batch from buffer base
	uint32_t batchStart = (uint8_t*)s_data.batchStartPtr - (uint8_t*)s_data.vertexBufferBase;
	uint32_t batchSize	= (uint8_t*)s_data.vertexPtr - (uint8_t*)s_data.batchStartPtr;

	s_data.vertexBuffer->setData(s_data.vertexBufferBase, batchSize + batchStart);
	s_data.indexBuffer->setData(s_data.indexBufferBase, s_data.indexCount);

	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
		RenderCommand::bindTexture(*s_data.textureSlots[i], i);
	}

	RenderCommand::bindPipeline(s_data.currentPipeline, *s_data.currentUniformBuffer);
	RenderCommand::bindVertexBuffer(*s_data.vertexBuffer, batchStart, 0);
	RenderCommand::drawIndexed(s_data.indexBuffer, s_data.indexCount);

	startNewBatch();
}

///////////////////////////////////////////////////////////////////////////////
//
// ImGui
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::beginImGui() {
	AT_PROFILE_FUNCTION();

	RenderCommand::beginImGui();
}

void Renderer::submitImGui() {
	AT_PROFILE_FUNCTION();

	RenderCommand::drawImGui();
}

///////////////////////////////////////////////////////////////////////////////
//
// Primitives
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawLine(glm::vec2 a, glm::vec2 b, glm::vec4 color, float thickness) {
	drawLine(a, b, 0.0f, color, thickness);
}

void Renderer::drawLine(glm::vec2 a, glm::vec2 b, float depth, glm::vec4 color, float thickness) {
	float length = glm::length(b - a);

	glm::vec2 size{length, thickness};
	glm::vec2 position{std::min(a.x, b.x), std::min(a.y, b.y)};
	float	  angle	   = atan2f(b.y - a.y, b.x - a.x);
	glm::vec2 midpoint = (a + b) * 0.5f;

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(midpoint, depth)) * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-length * 0.5f, -thickness * 0.5f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

	drawQuad(transform, color);
}

void Renderer::drawRectangle(glm::vec2 position, glm::vec2 size, glm::vec4 color, float thickness) {
	drawRectangle(position, size, 0.0f, color, thickness);
}

void Renderer::drawRectangle(glm::vec2 position, glm::vec2 size, float depth, glm::vec4 color, float thickness) {
	glm::vec2 top_left{position};
	glm::vec2 top_right{position.x + size.x, position.y};
	glm::vec2 bottom_left{position.x, position.y + size.y};
	glm::vec2 bottom_right{position.x + size.x, position.y + size.y};

	drawLine(top_left, top_right, depth, color, thickness);
	drawLine(top_left, bottom_left, depth, color, thickness);
	drawLine(top_right, bottom_right, depth, color, thickness);
	drawLine(bottom_left, bottom_right, depth, color, thickness);
}

void Renderer::drawCircle(glm::vec2 centerPosition, float radius, float depth, glm::vec4 color, float thickness, int segments) {
	glm::vec2 previous{centerPosition.x + radius, centerPosition.y};
	float	  angle = 0;
	for (int i = 0; i < segments; i++) {
		angle += 2 * M_PI / (float)segments;
		glm::vec2 point{centerPosition.x + radius * cos(angle), centerPosition.y + radius * sin(angle)};
		drawLine(point, previous, depth, color, thickness);
		previous = point;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Solid-color Polygons
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawFilledCircle(const glm::vec2& position, float radius, const glm::vec4& color, int segments) {
	drawFilledCircle(glm::vec3{position, 0}, radius, color, segments);
}

void Renderer::drawFilledCircle(const glm::vec3& position, float radius, const glm::vec4& color, int segments) {
	switchPipeline(s_data.polygonPipeline, s_data.polygonUniforms);

	glm::vec2 texCoords[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
	uint32_t  texIndex	   = 0;
	glm::mat4 transform	   = glm::translate(glm::mat4(1.0f), position);

	if (segments < 3) return;

	uint32_t baseVertex = s_data.vertexCount;

	// center vertex
	s_data.vertexPtr->position = position;
	s_data.vertexPtr->color	   = color;
	s_data.vertexPtr->texIndex = 0;
	s_data.vertexPtr->texCoord = {0, 0};
	s_data.vertexPtr++;

	// outer ring
	for (uint32_t i = 0; i <= segments; i++) {
		float angle = (float)i / (float)segments * 2.0f * M_PI;

		glm::vec3 p = {
			position.x + cos(angle) * radius,
			position.y + sin(angle) * radius,
			position.z};

		s_data.vertexPtr->position = p;
		s_data.vertexPtr->color	   = color;
		s_data.vertexPtr->texIndex = 0;
		s_data.vertexPtr->texCoord = {0, 0};
		s_data.vertexPtr++;
	}

	// indices (triangle fan)
	for (uint32_t i = 0; i < segments; i++) {
		s_data.indexPtr[0] = baseVertex;
		s_data.indexPtr[1] = baseVertex + i + 1;
		s_data.indexPtr[2] = baseVertex + i + 2;

		s_data.indexPtr += 3;
		s_data.indexCount += 3;
	}

	s_data.vertexCount += (segments + 2);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, color);
}

void Renderer::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
	glm::vec2 texCoords[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};

	switchPipeline(s_data.polygonPipeline, s_data.polygonUniforms);
	submitQuad(transform, color, 0, texCoords);
}

///////////////////////////////////////////////////////////////////////////////
//
// Textured Quads
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
	drawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, texture);
}

void Renderer::drawQuad(const glm::mat4& transform, const std::shared_ptr<Texture>& texture) {
	switchPipeline(s_data.polygonPipeline, s_data.polygonUniforms);

	glm::vec4 color	   = glm::vec4(1.0f);
	uint32_t  texIndex = getTextureIndex(texture);

	glm::vec2 texCoords[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};

	submitQuad(transform, color, texIndex, texCoords);
}

///////////////////////////////////////////////////////////////////////////////
//
// Subtexture Quads
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawQuad(const glm::vec2& position, const std::shared_ptr<SubTexture>& texture) {
	drawQuad(glm::vec3(position, 0.0f), texture->getSizeInTiles(), texture);
}

void Renderer::drawQuad(const glm::vec3& position, const std::shared_ptr<SubTexture>& texture) {
	drawQuad(position, texture->getSizeInTiles(), texture);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture) {
	drawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture) {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, texture);
}

void Renderer::drawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture>& texture) {
	switchPipeline(s_data.polygonPipeline, s_data.polygonUniforms);

	glm::vec4 color	   = glm::vec4(1.0f);
	uint32_t  texIndex = getTextureIndex(AssetManager::loadTexture(texture->getTexturePath()));

	TextureCoordinates tc			= texture->getTexCoords();
	glm::vec2		   texCoords[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};

	submitQuad(transform, color, texIndex, texCoords);
}

void Renderer::drawSprite(Component::Transform transform, Component::Sprite sprite) {
	switchPipeline(s_data.polygonPipeline, s_data.polygonUniforms);

	glm::mat4 trans = glm::translate(glm::mat4(1.0f), transform.position) * glm::scale(glm::mat4(1.0f), glm::vec3(transform.size, 1.0f));

	glm::vec4 color	   = glm::vec4(1.0f);
	uint32_t  texIndex = getTextureIndex(AssetManager::loadTexture(sprite.texturePath));

	TextureCoordinates tc = sprite.specs.coordinates;

	glm::vec2 texCoords[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};

	submitQuad(trans, color, texIndex, texCoords);
}

void Renderer::drawTilemap(Component::Transform transform, Component::Tilemap tilemap) {
	std::shared_ptr<Tileset> tileset = AssetManager::get<Tileset>(tilemap.tileset);
	std::shared_ptr<Texture> texture = AssetManager::loadTexture(tileset->getTexture());

	AT_CORE_ASSERT(ProjectManager::getActiveProject(), "Cannot render Tilemap: Active project is null");
	int	  tileSize		= ProjectManager::getActiveProject()->getData().tileSize;
	float worldTileSize = (float)tileSize / (float)ProjectManager::getActiveProject()->getData().pixelsPerUnit;

	glm::vec2 offset{0, 0};

	for (int i = 0; i < tilemap.size.y; i++) {
		float y_incr = 0.0;
		offset.x	 = 0;
		for (int j = 0; j < tilemap.size.x; j++) {
			int tileIndex = tilemap.getTile(j, i);

			float x_incr = worldTileSize;
			if (tileIndex != -1) {
				TileDefinition tile = tileset->getTileset().at(tileIndex);
				SubTexture	   tex{texture->getFilepath(), tileSize, tile.gridIndex, tile.sizeInTiles};

				drawQuad(
					{transform.position.x + offset.x, transform.position.y + offset.y, transform.position.z},
					glm::vec2(tile.sizeInTiles) * worldTileSize,
					std::make_shared<SubTexture>(tex));

				x_incr = tile.sizeInTiles.x * worldTileSize;
				if (y_incr < tile.sizeInTiles.y * worldTileSize)
					y_incr = tile.sizeInTiles.y * worldTileSize;
			}
			offset.x += x_incr;
		}
		offset.y += y_incr;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Text Rendering
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec2& position, uint32_t sizePx, const glm::vec4& color) {
	drawText(font, text, glm::vec3(position, 0.0f), sizePx, color);
}

void Renderer::drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec3& position, uint32_t sizePx, const glm::vec4& color) {
	float worldUnits = sizePx / s_pixelsPerWorldUnit;
	float scale		 = worldUnits / font->getFontSizePx();

	float xOffset = position.x;

	for (char c : text) {
		Font::Character ch = font->getCharacter(c);

		// Apply bearing so glyphs sit correctly on the baseline
		float xPos = xOffset + ch.bearing.x * scale;
		float yPos = position.y - ch.bearing.y * scale;
		// float yPos = position.y + (ch.size.y - ch.bearing.y) * scale; // descender correction

		glm::vec2 glyphSize = glm::vec2(ch.size.x * scale, ch.size.y * scale);

		drawCharacter(font, c, glm::vec3(xPos, yPos, position.z), glyphSize, color);

		// Advance stored in 1/64 pixels
		xOffset += (ch.advance >> 6) * scale;
	}
}

void Renderer::drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawCharacter(font, character, glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	switchPipeline(s_data.textPipeline, s_data.textUniforms);

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

	Font::Character c = font->getCharacter(character);

	uint32_t		   texIndex		= getTextureIndex(font->getTexture());
	TextureCoordinates tc			= c.texCoords;
	glm::vec2		   texCoords[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};

	submitQuad(transform, color, texIndex, texCoords);
}

}  // namespace Atlas