#pragma once

#include "Atlas/Core/Window.h"
#include "RendererAPI.h"

namespace Atlas {

class RenderCommand {
   public:
    inline static void setClearColor(const glm::vec4& color) { s_rendererAPI->setClearColor(color); }
    inline static void clear() { s_rendererAPI->clear(); }

    inline static void bindPipeline(std::shared_ptr<Pipeline> pipeline, const UniformBuffer& uBuffer) { s_rendererAPI->bindPipeline(pipeline, uBuffer); }
    inline static void bindTexture(const Texture& texture, uint32_t index) { s_rendererAPI->bindTexture(texture, index); }

    inline static void bindVertexBuffer(const VertexBuffer& vBuffer, int index = 0) { s_rendererAPI->bindVertexBuffer(vBuffer, index); }

    inline static void beginFrame() { s_rendererAPI->beginFrame(); }
    inline static void endFrame() { s_rendererAPI->endFrame(); }
    inline static void beginPass(std::shared_ptr<Framebuffer> framebuffer) { s_rendererAPI->beginPass(framebuffer); }
    inline static void endPass() { s_rendererAPI->endPass(); }

    inline static void beginImGui() { s_rendererAPI->beginImGui(); }
    inline static void drawImGui() { s_rendererAPI->drawImGui(); }

    inline static void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer) { s_rendererAPI->drawIndexed(indexBuffer); }
    inline static void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t count) { s_rendererAPI->drawIndexed(indexBuffer, count); }
    // inline static void drawPoint(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawPoint(vertexArray); }

    static void init(GraphicsContext& context);

   private:
    static RendererAPI* s_rendererAPI;
};

}  // namespace Atlas