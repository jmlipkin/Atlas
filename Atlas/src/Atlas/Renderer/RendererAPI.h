#pragma once

#include <glm/glm.hpp>

#include "Atlas/Core/Window.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Renderer/Pipeline.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/UniformBuffer.h"

namespace Atlas {

class RendererAPI {
   public:
    enum class API {
        None = 0,
        OpenGL = 1,
        Metal = 2
    };

   public:
    virtual void setClearColor(const glm::vec4& color) = 0;
    virtual void clear() = 0;
    virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
    virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t count) = 0;
    // virtual void drawPoint(const std::shared_ptr<VertexArray>& vertexArray) = 0;
    virtual void commit() = 0;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void beginImGui() = 0;
    virtual void drawImGui() = 0;

    virtual void bindPipeline(const Pipeline& pipeline, const UniformBuffer& uBuffer) = 0;
    virtual void bindTexture(const Texture& texture, uint32_t index) = 0;
    virtual void bindVertexBuffer(const VertexBuffer& vBuffer, int index) = 0;

    inline static API getAPI() { return s_API; }

   private:
    static API s_API;
};

}  // namespace Atlas