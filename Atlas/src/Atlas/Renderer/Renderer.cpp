#include "atpch.h"
#include "Renderer.h"

#include "Atlas/Core/Log.h"
#include "Atlas/Core/Font.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Atlas {

struct QuadVertex {
	glm::vec3 position;
	glm::vec4 color;

	uint32_t texIndex;
	glm::vec2 texCoord;
};

struct RendererData {
	const uint32_t maxQuadCount = 10000;
	const uint32_t maxVertexCount = maxQuadCount * 4;
	const uint32_t maxIndexCount = maxQuadCount * 6;
	static const uint32_t maxTextureSlots = 32;

	std::shared_ptr<Pipeline> pointPipeline;
	std::shared_ptr<UniformBuffer> pointUniforms;

	std::shared_ptr<VertexBuffer> quadVertexBuffer;
	std::shared_ptr<IndexBuffer> quadIndexBuffer;
	std::shared_ptr<Pipeline> quadPipeline;
	std::shared_ptr<UniformBuffer> quadUniforms;

	std::shared_ptr<Pipeline> textPipeline;
	std::shared_ptr<UniformBuffer> textUniforms;

	std::shared_ptr<ShaderLibrary> shaderLib;

	std::array<std::shared_ptr<Texture>, maxTextureSlots> textureSlots;
	uint32_t textureSlotIndex = 1;	// 0 = white texture, 1 = invalid texture

	uint32_t quadIndexCount = 0;
	QuadVertex* quadVertexBufferBase = nullptr;
	QuadVertex* quadVertexPtr = nullptr;
	QuadVertex* batchStartPtr = nullptr;
	std::shared_ptr<Pipeline> currentPipeline = nullptr;
	std::shared_ptr<UniformBuffer> currentUniformBuffer = nullptr;

	float pixelsPerWorldUnit;

	glm::vec4 quadVertexPositions[4];
};

static RendererData s_data;

void Renderer::init(GraphicsContext& context) {
	AT_PROFILE_FUNCTION();

	RenderCommand::init(context);

	std::string filepath = "examples/PacMan/src/shaders.metallib";
	s_data.shaderLib = ShaderLibrary::create(filepath);
	s_data.shaderLib->load("Quad Shader", "quadVertexShader", "quadFragmentShader");

	s_data.shaderLib->load("Text Shader", "textVertexShader", "textFragmentShader");

#ifdef POINT_BATCHING_NOT_IMPLEMENTED_YET
	s_data.shaderLib->load("Point Shader", "pointVertexShader", "pointFragmentShader");
	float pointVertices[1 * 1] = {
		// 0.0f, 0.0f, 0.0f, // position
		20.0f  // size
	};
	s_data.pointVertexArray.reset(VertexArray::create());
	std::shared_ptr<VertexBuffer> pointVB = VertexBuffer::create(pointVertices, sizeof(pointVertices));
	s_data.pointVertexArray->addVertexBuffer(pointVB);

	uint32_t pointIndices[1] = {0};
	std::shared_ptr<IndexBuffer> pointIB = IndexBuffer::create(pointIndices, sizeof(pointIndices));
	s_data.pointVertexArray->setIndexBuffer(pointIB);

	PipelineSpecification pointPipelineSpecs;
	pointPipelineSpecs.name = "Point Pipeline";
	pointPipelineSpecs.shader = s_data.shaderLib->get("Point Shader");
	pointPipelineSpecs.layout = BufferLayout({/*{"a_Position", ShaderDataType::Float3}, */ {"a_Size", ShaderDataType::Float}});

	s_data.pointUniforms = UniformBuffer::create(
		pointPipelineSpecs, {{"u_position", 0, glm::vec3(0.0f)}, {"u_viewProjection", 1, glm::mat4(1.0f)}, {"u_color", 2, glm::vec4(1.0f)}},
		1);
	s_data.pointPipeline = Pipeline::create(pointPipelineSpecs);

#endif

	{
		AT_PROFILE_SCOPE("Allocate QuadVertex data");

		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertexCount];
	}

	s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertexCount * sizeof(QuadVertex));

	uint32_t* quadIndices = new uint32_t[s_data.maxIndexCount];
	uint32_t offset = 0;

	{
		AT_PROFILE_SCOPE("Create QuadIndex Data");

		for (uint32_t i = 0; i < s_data.maxIndexCount; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
	}

	s_data.quadIndexBuffer = IndexBuffer::create(quadIndices, s_data.maxIndexCount);

	PipelineSpecification quadPipelineSpecs;
	quadPipelineSpecs.name = "Quad Pipeline";
	quadPipelineSpecs.shader = s_data.shaderLib->get("Quad Shader");
	quadPipelineSpecs.layout = BufferLayout({{"a_position", ShaderDataType::Float3},
											 {"a_color", ShaderDataType::Float4},
											 {"a_texIndex", ShaderDataType::Uint},
											 {"a_texCoord", ShaderDataType::Float2}});

	s_data.quadUniforms = UniformBuffer::create(
		quadPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
		1);

	s_data.quadPipeline = Pipeline::create(quadPipelineSpecs);

	PipelineSpecification textPipelineSpecs;
	textPipelineSpecs.name = "Text Pipeline";
	textPipelineSpecs.shader = s_data.shaderLib->get("Text Shader");
	textPipelineSpecs.enableDepthWrite = false;
	textPipelineSpecs.layout = BufferLayout({{"a_position", ShaderDataType::Float3},
											 {"a_color", ShaderDataType::Float4},
											 {"a_texIndex", ShaderDataType::Uint},
											 {"a_texCoord", ShaderDataType::Float2}});

	s_data.textUniforms = UniformBuffer::create(
		textPipelineSpecs, {{"u_viewProjection", 0, glm::mat4(1.0f)}},
		2);
	s_data.textPipeline = Pipeline::create(textPipelineSpecs);

	std::shared_ptr<Texture> whiteTexture = Texture::create(1, 1);
	uint32_t whiteTextureData = 0xFFFFFFFF;
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

	delete s_data.quadVertexBufferBase;
}

void Renderer::beginScene(const OrthographicCamera& camera) {
	AT_PROFILE_FUNCTION();

	s_data.quadUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
	s_data.textUniforms->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

	s_data.quadVertexPtr = s_data.quadVertexBufferBase;
	s_data.batchStartPtr = s_data.quadVertexBufferBase;

	s_data.pixelsPerWorldUnit = camera.getPixelsPerWorldUnit();

	startNewBatch();
}

void Renderer::endScene() {
	AT_PROFILE_FUNCTION();

	flush();
}

void Renderer::startNewBatch() {
	AT_PROFILE_FUNCTION();

	s_data.quadIndexCount = 0;
	s_data.batchStartPtr = s_data.quadVertexPtr;

	s_data.textureSlotIndex = 1;

	s_data.currentPipeline = nullptr;
	s_data.currentUniformBuffer = nullptr;
}

void Renderer::flush() {
	AT_PROFILE_FUNCTION();

	if (s_data.quadIndexCount == 0 || s_data.currentPipeline == nullptr || s_data.currentUniformBuffer == nullptr) {
		startNewBatch();
		return;
	}

	// Calculate offset of current batch from buffer base
	uint32_t batchStart = (uint8_t*)s_data.batchStartPtr - (uint8_t*)s_data.quadVertexBufferBase;
	uint32_t batchSize = (uint8_t*)s_data.quadVertexPtr - (uint8_t*)s_data.batchStartPtr;

	s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, batchSize + batchStart);

	for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
		RenderCommand::bindTexture(*s_data.textureSlots[i], i);
	}

	RenderCommand::bindPipeline(s_data.currentPipeline, *s_data.currentUniformBuffer);
	RenderCommand::bindVertexBuffer(*s_data.quadVertexBuffer, batchStart, 0);

	RenderCommand::drawIndexed(s_data.quadIndexBuffer, s_data.quadIndexCount);

	startNewBatch();
}

void Renderer::beginImGui() {
	AT_PROFILE_FUNCTION();

	RenderCommand::beginImGui();
}

void Renderer::submitImGui() {
	AT_PROFILE_FUNCTION();

	RenderCommand::drawImGui();
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	drawQuad(glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, color);
}

void Renderer::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
	if (s_data.currentPipeline != s_data.quadPipeline) {
		flush();
		s_data.currentPipeline = s_data.quadPipeline;
		s_data.currentUniformBuffer = s_data.quadUniforms;
	}

	if (s_data.quadIndexCount >= s_data.maxIndexCount)
		flush();

	uint32_t texIndex = 0;
	glm::vec2 texCoordinates[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
	constexpr size_t quadVertexCount = 4;

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_data.quadVertexPtr->position = transform * s_data.quadVertexPositions[i];
		s_data.quadVertexPtr->color = color;
		s_data.quadVertexPtr->texIndex = texIndex;
		s_data.quadVertexPtr->texCoord = texCoordinates[i];
		s_data.quadVertexPtr++;
	}

	s_data.quadIndexCount += 6;
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
	drawQuad(glm::vec3(position, 0.0f), size, texture);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
	AT_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, texture);
}

void Renderer::drawQuad(const glm::mat4& transform, const std::shared_ptr<Texture>& texture) {
	AT_PROFILE_FUNCTION();

	if (s_data.currentPipeline != s_data.quadPipeline) {
		flush();
		s_data.currentPipeline = s_data.quadPipeline;
		s_data.currentUniformBuffer = s_data.quadUniforms;
	}

	glm::vec4 color = glm::vec4(1.0f);
	uint32_t texIndex = 0;

	for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
		if (*texture.get() == *s_data.textureSlots[i].get()) {
			texIndex = i;
			break;
		}
	}
	if (texIndex == 0) {
		if (s_data.textureSlotIndex >= s_data.maxTextureSlots) {
			flush();
		}

		texIndex = s_data.textureSlotIndex;
		s_data.textureSlots[texIndex] = texture;
		s_data.textureSlotIndex++;
	}
	if (s_data.quadIndexCount >= s_data.maxIndexCount)
		flush();

	glm::vec2 texCoordinates[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
	constexpr size_t quadVertexCount = 4;

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_data.quadVertexPtr->position = transform * s_data.quadVertexPositions[i];
		s_data.quadVertexPtr->color = color;
		s_data.quadVertexPtr->texIndex = texIndex;
		s_data.quadVertexPtr->texCoord = texCoordinates[i];
		s_data.quadVertexPtr++;
	}

	s_data.quadIndexCount += 6;
}

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
	AT_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawQuad(transform, texture);
}

void Renderer::drawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture>& texture) {
	AT_PROFILE_FUNCTION();

	if (s_data.currentPipeline != s_data.quadPipeline) {
		flush();
		s_data.currentPipeline = s_data.quadPipeline;
		s_data.currentUniformBuffer = s_data.quadUniforms;
	}

	glm::vec4 color = glm::vec4(1.0f);
	uint32_t texIndex = 0;

	for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
		if (*texture->getTexture() == *s_data.textureSlots[i].get()) {
			texIndex = i;
			break;
		}
	}
	if (texIndex == 0) {
		if (s_data.textureSlotIndex >= s_data.maxTextureSlots) {
			flush();
		}

		texIndex = s_data.textureSlotIndex;
		s_data.textureSlots[texIndex] = texture->getTexture();
		s_data.textureSlotIndex++;
	}
	if (s_data.quadIndexCount >= s_data.maxIndexCount)
		flush();

	TextureCoordinates tc = texture->getTexCoords();
	glm::vec2 texCoordinates[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};
	constexpr size_t quadVertexCount = 4;

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_data.quadVertexPtr->position = transform * s_data.quadVertexPositions[i];
		s_data.quadVertexPtr->color = color;
		s_data.quadVertexPtr->texIndex = texIndex;
		s_data.quadVertexPtr->texCoord = texCoordinates[i];
		s_data.quadVertexPtr++;
	}

	s_data.quadIndexCount += 6;
}

void Renderer::drawQuad(const glm::vec2& position, const std::shared_ptr<TextureSheet>& sheet) {
	drawQuad(glm::vec3(position, 0.0f), sheet->getSizeInTiles(), sheet);
}

void Renderer::drawQuad(const glm::vec3& position, const std::shared_ptr<TextureSheet>& sheet) {
	drawQuad(position, sheet->getSizeInTiles(), sheet);
}

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<TextureSheet>& sheet) {
	drawQuad(glm::vec3(position, 0.0f), size, sheet);
}

void Renderer::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<TextureSheet>& sheet) {
	AT_PROFILE_FUNCTION();

	if (s_data.currentPipeline != s_data.quadPipeline) {
		flush();
		s_data.currentPipeline = s_data.quadPipeline;
		s_data.currentUniformBuffer = s_data.quadUniforms;
	}

	glm::vec4 color = glm::vec4(1.0f);
	uint32_t texIndex = 0;

	for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
		if (*sheet->getTexture() == *s_data.textureSlots[i].get()) {
			texIndex = i;
			break;
		}
	}
	if (texIndex == 0) {
		if (s_data.textureSlotIndex >= s_data.maxTextureSlots) {
			flush();
		}

		texIndex = s_data.textureSlotIndex;
		s_data.textureSlots[texIndex] = sheet->getTexture();
		s_data.textureSlotIndex++;
	}

	s_data.quadVertexPtr->position = {position.x, position.y + size.y, position.z};
	s_data.quadVertexPtr->color = color;
	s_data.quadVertexPtr->texIndex = texIndex;
	s_data.quadVertexPtr->texCoord = sheet->getTexCoords().bottom_left;
	s_data.quadVertexPtr++;

	s_data.quadVertexPtr->position = {position.x + size.x, position.y + size.y, position.z};
	s_data.quadVertexPtr->color = color;
	s_data.quadVertexPtr->texIndex = texIndex;
	s_data.quadVertexPtr->texCoord = sheet->getTexCoords().bottom_right;
	s_data.quadVertexPtr++;

	s_data.quadVertexPtr->position = {position.x + size.x, position.y, position.z};
	s_data.quadVertexPtr->color = color;
	s_data.quadVertexPtr->texIndex = texIndex;
	s_data.quadVertexPtr->texCoord = sheet->getTexCoords().top_right;
	s_data.quadVertexPtr++;

	s_data.quadVertexPtr->position = {position.x, position.y, position.z};
	s_data.quadVertexPtr->color = color;
	s_data.quadVertexPtr->texIndex = texIndex;
	s_data.quadVertexPtr->texCoord = sheet->getTexCoords().top_left;
	s_data.quadVertexPtr++;

	s_data.quadIndexCount += 6;
}

void Renderer::drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec2& position, uint32_t sizePx, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	drawText(font, text, glm::vec3(position, 0.0f), sizePx, color);
}

void Renderer::drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec3& position, uint32_t sizePx, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	float worldUnits = sizePx / s_data.pixelsPerWorldUnit;
	float scale = worldUnits / font->getFontSizePx();

	float xOffset = position.x;

	for(char c : text) {
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

void Renderer::drawCharacter(const std::shared_ptr<Font> &font, char character, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	drawCharacter(font, character, glm::vec3(position, 0.0f), size, color);
}

void Renderer::drawCharacter(const std::shared_ptr<Font> &font, char character, const glm::vec3 &position, const glm::vec2 &size, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
	drawCharacter(font, character, transform, color);
}

void Renderer::drawCharacter(const std::shared_ptr<Font> &font, char character, const glm::mat4 &transform, const glm::vec4& color) {
	AT_PROFILE_FUNCTION();

	Font::Character c = font->getCharacter(character);

	if (s_data.currentPipeline != s_data.textPipeline) {
		flush();
		s_data.currentPipeline = s_data.textPipeline;
		s_data.currentUniformBuffer = s_data.textUniforms;
	}

	uint32_t texIndex = 0;

	for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
		if (*font->getTexture() == *s_data.textureSlots[i].get()) {
			texIndex = i;
			break;
		}
	}
	if (texIndex == 0) {
		if (s_data.textureSlotIndex >= s_data.maxTextureSlots) {
			flush();
		}

		texIndex = s_data.textureSlotIndex;
		s_data.textureSlots[texIndex] = font->getTexture();
		s_data.textureSlotIndex++;
	}
	if (s_data.quadIndexCount >= s_data.maxIndexCount)
		flush();

	TextureCoordinates tc = c.texCoords;
	glm::vec2 texCoordinates[4] = {tc.bottom_left, tc.bottom_right, tc.top_right, tc.top_left};
	constexpr size_t quadVertexCount = 4;

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_data.quadVertexPtr->position = transform * s_data.quadVertexPositions[i];
		s_data.quadVertexPtr->color = color;
		s_data.quadVertexPtr->texIndex = texIndex;
		s_data.quadVertexPtr->texCoord = texCoordinates[i];
		s_data.quadVertexPtr++;
	}

	s_data.quadIndexCount += 6;

}

}  // namespace Atlas