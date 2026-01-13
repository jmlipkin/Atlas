#pragma once

#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"

namespace Atlas {

    class OpenGLRendererAPI : public RendererAPI {
       public:
        virtual ~OpenGLRendererAPI() = default;
        virtual void setClearColor(const glm::vec4& color) override;
        virtual void clear() override;
        
        // TODO: Implement
        virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer) override {}
        virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t count) override {}
        virtual void beginImGui() override {}
        virtual void drawImGui() override {}
        void bindPipeline(const Pipeline& pipeline, const UniformBuffer& uBuffer) override {}
        void bindTexture(const Texture& texture, uint32_t index) override {}
        void bindVertexBuffer(const VertexBuffer& buffer, int index) override {}
        virtual void beginFrame() override;
        virtual void endFrame() override {}
        virtual void commit() override {}
    };

}  // namespace Atlas