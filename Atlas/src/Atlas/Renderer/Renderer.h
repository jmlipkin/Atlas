#pragma once

#include "OrthographicCamera.h"
#include "Pipeline.h"
#include "RenderCommand.h"
#include "Texture.h"
namespace Atlas {

class Renderer {
   public:
    static void init(GraphicsContext& context);
    static void shutdown();

    static void beginScene(const OrthographicCamera& camere);
    static void endScene();

    static void beginImGui();
    static void submitImGui();

    static void submit(const Pipeline& pipeline, const std::shared_ptr<VertexArray>& vertexArray, const UniformBuffer& uBuffer);

    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);
    static void drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);

    inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

   private:
    static void startNewBatch();
    static void flush();
};

}  // namespace Atlas