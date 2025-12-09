#pragma once
#include "Atlas/Renderer/RendererAPI.h"

#include "MetalContext.h"

namespace Atlas {

    class MetalRendererAPI : public RendererAPI {
    public:
     MetalRendererAPI();
     virtual ~MetalRendererAPI();
     virtual void setClearColor(const glm::vec4& color) override;
     virtual void clear() override;
     virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
     virtual void commit() override;

    private:
     MTL::Device& m_pDevice;
     MTL::RenderPassDescriptor* m_renderPDescriptor;
     MTL::CommandBuffer* m_commandBuffer;
     MTL::CommandQueue* m_commandQueue;
     CA::MetalDrawable* m_drawable;
    };

}  // namespace Atlas