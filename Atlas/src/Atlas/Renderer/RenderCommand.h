#pragma once

#include "Atlas/Core/Window.h"
#include "RendererAPI.h"

namespace Atlas {

class RenderCommand {
   public:
    inline static void setClearColor(const glm::vec4& color) { s_rendererAPI->setClearColor(color); }
    inline static void clear() { s_rendererAPI->clear(); }
    inline static void commit() { s_rendererAPI->commit(); }

    inline static void bindPipeline(const Pipeline& pipeline, const UniformBuffer& uBuffer) { s_rendererAPI->bindPipeline(pipeline, uBuffer); }
    inline static void bindTexture(const Pipeline& pipeline, const Texture& texture, uint32_t index) { s_rendererAPI->bindTexture(pipeline, texture, index); }

    inline static void bindVertexArray(const VertexArray& array) { s_rendererAPI->bindVertexArray(array); }
    inline static void bindVertexBuffer(const VertexBuffer& vBuffer, int index = 0) { s_rendererAPI->bindVertexBuffer(vBuffer, index); }

    inline static void beginFrame() { s_rendererAPI->beginFrame(); }
    inline static void endFrame() { s_rendererAPI->endFrame(); }

    inline static void beginImGui() { s_rendererAPI->beginImGui(); }
    inline static void drawImGui() { s_rendererAPI->drawImGui(); }

    inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawIndexed(vertexArray); }
    inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) { s_rendererAPI->drawIndexed(vertexArray, count); }
    inline static void drawPoint(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawPoint(vertexArray); }

    static void init(GraphicsContext& context);

   private:
    static RendererAPI* s_rendererAPI;
};

}  // namespace Atlas