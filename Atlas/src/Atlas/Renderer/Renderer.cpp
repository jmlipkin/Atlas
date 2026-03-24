#include "atpch.h"
#include "Renderer.h"

#include "Atlas/Core/Log.h"
#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Font.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"
#include "glm/detail/func_geometric.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Atlas {

struct QuadVertex {
	glm::vec3 position;
	glm::vec4 color;
	uint32_t  texIndex;
	glm::vec2 texCoord;
};

struct RendererData {
	const uint32_t maxQuadCount	  = 10000;
	const uint32_t maxVertexCount = maxQuadCount * 4;
	const uint32_t maxIndexCount  = maxQuadCount * 6;

	static const uint32_t maxTextureSlots = 32;

	std::shared_ptr<VertexBuffer>  quadVertexBuffer;
	std::shared_ptr<IndexBuffer>   quadIndexBuffer;
	std::shared_ptr<Pipeline>	   quadPipeline;
	std::shared_ptr<UniformBuffer> quadUniforms;

	std::shared_ptr<Pipeline>	   textPipeline;
	std::shared_ptr<UniformBuffer> textUniforms;

	std::array<std::shared_ptr<Texture>, maxTextureSlots> textureSlots;

	uint32_t textureSlotIndex = 1;	// 0 = white texture, 1 = invalid texture

	uint32_t	quadIndexCount		 = 0;
	QuadVertex* quadVertexBufferBase = nullptr;
	QuadVertex* quadVertexPtr		 = nullptr;
	QuadVertex* batchStartPtr		 = nullptr;

	std::shared_ptr<Pipeline>	   currentPipeline		= nullptr;
	std::shared_ptr<UniformBuffer> currentUniformBuffer = nullptr;

	glm::vec4 quadVertexPositions[4];
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
	if (s_data.quadIndexCount >= s_data.maxIndexCount)
		flush();

	for (size_t i = 0; i < 4; i++) {
		s_data.quadVertexPtr->position = transform * s_data.quadVertexPositions[i];
		s_data.quadVertexPtr->color	   = color;
		s_data.quadVertexPtr->texIndex = texIndex;
		s_data.quadVertexPtr->texCoord = texCoords[i];
		s_data.quadVertexPtr++;
	}

	s_data.quadIndexCount += 6;
}

///////////////////////////////////////////////////////////////////////////////
//
// Lifecycle
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::init(GraphicsContext& context) {
	AT_PROFILE_FUNCTION();

	RenderCommand::init(context);

	AssetManager::loadShader("Quad Shader", "quadVertexShader", "quadFragmentShader");
	AssetManager::loadShader("Text Shader", "textVertexShader", "textFragmentShader");

	s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertexCount];
	s_data.quadVertexBuffer		= VertexBuffer::create(s_data.maxVertexCount * sizeof(QuadVertex));

	// Quad index buffer (used for text also)
	uint32_t* quadIndices = new uint32_t[s_data.maxIndexCount];
	for (uint32_t i = 0, offset = 0; i < s_data.maxIndexCount; i += 6, offset += 4) {
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;
	}
	s_data.quadIndexBuffer = IndexBuffer::create(quadIndices, s_data.maxIndexCount);
	delete[] quadIndices;

	// Quad pipeline
	PipelineSpecification quadPipelineSpecs;
	quadPipelineSpecs.name	 = "Quad Pipeline";
	quadPipelineSpecs.shader = AssetManager::get<Shader>("Quad Shader");
	quadPipelineSpecs.layout = BufferLayout({{"a_position", ShaderDataType::Float3},
											 {"a_color", ShaderDataType::Float4},
											 {"a_texIndex", ShaderDataType::Uint},
											 {"a_texCoord", ShaderDataType::Float2}});

	s_data.quadUniforms = UniformBuffer::create(
		quadPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
		1);

	s_data.quadPipeline = Pipeline::create(quadPipelineSpecs);

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

	s_data.quadVertexPositions[0] = {0.0f, 1.0f, 0.0f, 1.0f};
	s_data.quadVertexPositions[1] = {1.0f, 1.0f, 0.0f, 1.0f};
	s_data.quadVertexPositions[2] = {1.0f, 0.0f, 0.0f, 1.0f};
	s_data.quadVertexPositions[3] = {0.0f, 0.0f, 0.0f, 1.0f};

	AT_CORE_TRACE("Renderer initialized");
}

void Renderer::shutdown() {
	AT_PROFILE_FUNCTION();

	delete[] s_data.quadVertexBufferBase;
}

void Renderer::beginScene(const OrthographicCamera& camera) {
	AT_PROFILE_FUNCTION();

	s_data.quadUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
	s_data.textUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

	s_data.quadVertexPtr = s_data.quadVertexBufferBase;
	s_data.batchStartPtr = s_data.quadVertexBufferBase;

	s_pixelsPerWorldUnit = camera.getPixelsPerWorldUnit();

	startNewBatch();
}

void Renderer::endScene() {
	AT_PROFILE_FUNCTION();

	flush();
}

void Renderer::startNewBatch() {
	AT_PROFILE_FUNCTION();

	s_data.quadIndexCount	= 0;
	s_data.batchStartPtr	= s_data.quadVertexPtr;
	s_data.textureSlotIndex = 1;

	s_data.currentPipeline		= nullptr;
	s_data.currentUniformBuffer = nullptr;
}

void Renderer::flush() {
	AT_PROFILE_FUNCTION();

	if (s_data.quadIndexCount == 0 || !s_data.currentPipeline || !s_data.currentUniformBuffer) {
		startNewBatch();
		return;
	}

	// Calculate offset of current batch from buffer base
	uint32_t batchStart = (uint8_t*)s_data.batchStartPtr - (uint8_t*)s_data.quadVertexBufferBase;
	uint32_t batchSize	= (uint8_t*)s_data.quadVertexPtr - (uint8_t*)s_data.batchStartPtr;

	s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, batchSize + batchStart);

	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
		RenderCommand::bindTexture(*s_data.textureSlots[i], i);
	}

	RenderCommand::bindPipeline(s_data.currentPipeline, *s_data.currentUniformBuffer);
	RenderCommand::bindVertexBuffer(*s_data.quadVertexBuffer, batchStart, 0);
	RenderCommand::drawIndexed(s_data.quadIndexBuffer, s_data.quadIndexCount);

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
// Solid-color Quads
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, color);
}

void Renderer::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
	glm::vec2 texCoords[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};

	switchPipeline(s_data.quadPipeline, s_data.quadUniforms);
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
	switchPipeline(s_data.quadPipeline, s_data.quadUniforms);

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
	switchPipeline(s_data.quadPipeline, s_data.quadUniforms);

	glm::vec4 color	   = glm::vec4(1.0f);
	uint32_t  texIndex = getTextureIndex(AssetManager::loadTexture(texture->getTexturePath()));

	TextureCoordinates tc			= texture->getTexCoords();
	glm::vec2		   texCoords[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};

	submitQuad(transform, color, texIndex, texCoords);
}

void Renderer::drawSprite(Component::Transform transform, Component::Sprite sprite) {
	switchPipeline(s_data.quadPipeline, s_data.quadUniforms);

	glm::mat4 trans = glm::translate(glm::mat4(1.0f), transform.position) * glm::scale(glm::mat4(1.0f), glm::vec3(transform.size, 1.0f));

	glm::vec4 color	   = glm::vec4(1.0f);
	uint32_t  texIndex = getTextureIndex(AssetManager::loadTexture(sprite.texturePath));

	TextureCoordinates tc = sprite.specs.coordinates;

	glm::vec2 texCoords[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};

	submitQuad(trans, color, texIndex, texCoords);
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