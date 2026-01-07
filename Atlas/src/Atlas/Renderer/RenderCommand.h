#pragma once

#include "Atlas/Core/Window.h"
#include "RendererAPI.h"

namespace Atlas {

class RenderCommand {
   public:
    inline static void setClearColor(const glm::vec4& color) { s_rendererAPI->setClearColor(color); }
    inline static void clear() { s_rendererAPI->clear(); }
    inline static void commit() { s_rendererAPI->commit(); }

    inline static void bindPipeline(const Pipeline& pipeline) { s_rendererAPI->bindPipeline(pipeline); }

    inline static void bindVertexArray(const VertexArray& array) { s_rendererAPI->bindVertexArray(array); }
    inline static void bindVertexBuffer(const VertexBuffer& vBuffer, int index = 0) { s_rendererAPI->bindVertexBuffer(vBuffer, index); }

    inline static void beginFrame() { s_rendererAPI->beginFrame(); }
    inline static void endFrame() { s_rendererAPI->endFrame(); }

    inline static void beginImGui() { s_rendererAPI->beginImGui(); }
    inline static void drawImGui() { s_rendererAPI->drawImGui(); }

    inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawIndexed(vertexArray); }

    inline static void setBool(const std::string& name, const bool value) {s_rendererAPI->setBool(name, value); }
    inline static void setInt(const std::string& name, const int value) {s_rendererAPI->setInt(name, value); }
    inline static void setFloat(const std::string& name, const float value) {s_rendererAPI->setFloat(name, value); }
    inline static void setFloat2(const std::string& name, const glm::vec2& value) {s_rendererAPI->setFloat2(name, value); }
    inline static void setFloat3(const std::string& name, const glm::vec3& value) {s_rendererAPI->setFloat3(name, value); }
    inline static void setFloat4(const std::string& name, const glm::vec4& value) {s_rendererAPI->setFloat4(name, value); }
    inline static void setMat3(const std::string& name, const glm::mat3& value) {s_rendererAPI->setMat3(name, value); }
    inline static void setMat4(const std::string& name, const glm::mat4& value) {s_rendererAPI->setMat4(name, value); }

    static void init(GraphicsContext& context);

   private:
    static RendererAPI* s_rendererAPI;
};

}  // namespace Atlas