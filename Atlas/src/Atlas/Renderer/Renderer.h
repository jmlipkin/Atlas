#pragma once

#include "Atlas/Core/Font.h"

#include "Atlas/Renderer/GraphicsContext.h"
#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"
#include "Atlas/Renderer/OrthographicCamera.h"
#include "Atlas/Renderer/UniformBuffer.h"

#include "Atlas/ECS/Components/Components.h"

namespace Atlas {

class Renderer {
  public:
	static void init(GraphicsContext& context);
	static void shutdown();

	static void beginScene(const OrthographicCamera& camera);
	static void endScene();

	static void beginImGui();
	static void submitImGui();

	static void drawLine(glm::vec2 a, glm::vec2 b, glm::vec4 color, float thickness = 1.0f);
	static void drawLine(glm::vec2 a, glm::vec2 b, float depth, glm::vec4 color, float thickness = 1.0f);

	static void drawRectangle(glm::vec2 position, glm::vec2 size, glm::vec4 color, float thickness);
	static void drawRectangle(glm::vec2 position, glm::vec2 size, float depth, glm::vec4 color, float thickness);

	static void drawCircle(glm::vec2 centerPosition, float radius, float depth, glm::vec4 color, float thickness = 1.0f, int segments = 128);
	static void drawFilledCircle(const glm::vec2& center, float radius, const glm::vec4& color = glm::vec4(1.0f), int segments = 128);
	static void drawFilledCircle(const glm::vec3& center, float radius, const glm::vec4& color = glm::vec4(1.0f), int segments = 128);

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);

	static void drawQuad(const glm::vec2& position, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec3& position, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture);

	static void drawSprite(Component::Transform transform, Component::Sprite sprite);
	static void drawTilemap(Component::Transform transform, Component::Tilemap tilemap);

	static void drawQuad(const glm::mat4& transform, const glm::vec4& color);
	static void drawQuad(const glm::mat4& transform, const std::shared_ptr<Texture>& texture);
	static void drawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture>& texture);

	static void drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec2& position, uint32_t sizePx, const glm::vec4& color = glm::vec4(1.0f));
	static void drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec3& position, uint32_t sizePx, const glm::vec4& color = glm::vec4(1.0f));

	static void drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));
	static void drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f));

	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

  private:
	static void startNewBatch();
	static void flush();

	static void		switchPipeline(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<UniformBuffer> uniforms);
	static uint32_t getTextureIndex(const std::shared_ptr<Texture>& texture);
	static void		submitQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t texIndex, const glm::vec2 texCoords[4]);
};

}  // namespace Atlas