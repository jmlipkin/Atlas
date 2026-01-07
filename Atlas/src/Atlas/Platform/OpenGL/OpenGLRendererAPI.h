#pragma once

#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"

namespace Atlas {

    class OpenGLRendererAPI : public RendererAPI {
       public:
        virtual ~OpenGLRendererAPI() = default;
        virtual void setClearColor(const glm::vec4& color) override;
        virtual void clear() override;
        virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
        
        // TODO: Implement
        virtual void beginImGui() override {}
        virtual void drawImGui() override {}
        void bindPipeline(const Pipeline& pipeline) override {}
        void bindVertexArray(const VertexArray& array) override {}
        void bindVertexBuffer(const VertexBuffer& buffer, int index) override {}
        virtual void beginFrame() override;
        virtual void endFrame() override {}
        virtual void commit() override {}
    };

}  // namespace Atlas