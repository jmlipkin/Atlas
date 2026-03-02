#pragma once

#include "Atlas/Core/Font.h"
#include "Atlas/Renderer/RenderCommand.h"

#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/TextureSheet.h"
#include "Atlas/Renderer/OrthographicCamera.h"

namespace Atlas {

class Renderer {
  public:
	static void init(GraphicsContext& context);
	static void shutdown();

	static void beginScene(const OrthographicCamera& camera);
	static void endScene();

	static void beginImGui();
	static void submitImGui();

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);

	static void drawQuad(const glm::vec2& position, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec3& position, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& texture);

	static void drawQuad(const glm::vec2& position, const std::shared_ptr<TextureSheet>& texture);
	static void drawQuad(const glm::vec3& position, const std::shared_ptr<TextureSheet>& texture);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<TextureSheet>& texture);
	static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<TextureSheet>& texture);

	static void drawQuad(const glm::mat4& transform, const glm::vec4& color);
	static void drawQuad(const glm::mat4& transform, const std::shared_ptr<Texture>& texture);
	static void drawQuad(const glm::mat4& transform, const std::shared_ptr<SubTexture>& texture);

	static void drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec2& position, const glm::vec2& size);
	static void drawText(const std::shared_ptr<Font>& font, const std::string& text, const glm::vec3& position, const glm::vec2& size);

	static void drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec2& position, const glm::vec2& size);
	static void drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::vec3& position, const glm::vec2& size);
	static void drawCharacter(const std::shared_ptr<Font>& font, char character, const glm::mat4& transform);

	inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

  private:
	static void startNewBatch();
	static void flush();
};

}  // namespace Atlas