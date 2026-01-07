#pragma once

#include <glm/glm.hpp>

#include "Atlas/Window.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Renderer/Pipeline.h"
#include "Atlas/Renderer/VertexArray.h"

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
    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
    virtual void commit() = 0;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void bindPipeline(const Pipeline& pipeline) = 0;
    virtual void bindVertexArray(const VertexArray& array) = 0;
    virtual void bindVertexBuffer(const VertexBuffer& vBuffer, int index) = 0;

    inline static API getAPI() { return s_API; }

   private:
    static API s_API;
};

}  // namespace Atlas