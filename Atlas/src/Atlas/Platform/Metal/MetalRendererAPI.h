#pragma once

#include <CoreGraphics/CGEventTypes.h>
#include "Atlas/Renderer/RendererAPI.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include "Atlas/Platform/Metal/MetalFramebuffer.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalRendererAPI : public RendererAPI {
   public:
    MetalRendererAPI(MetalContext& context);
    virtual ~MetalRendererAPI();

    virtual void setClearColor(const glm::vec4& color) override;
    virtual void clear() override;

    virtual void beginFrame() override;
	virtual void endFrame() override;
    virtual void beginPass(std::shared_ptr<Framebuffer> framebuffer) override;
	virtual void endPass() override;

    // ImGui functions implemented in MetalRendererAPI.mm
    virtual void beginImGui() override;
    virtual void drawImGui() override;

    void bindPipeline(std::shared_ptr<Pipeline> pipeline, const UniformBuffer& uBuffer) override;
    void bindTexture(const Texture& texture, uint32_t index) override;
    void bindVertexBuffer(const VertexBuffer& buffer, uint32_t offset, int index) override;

    virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
    virtual void drawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t count) override;
    // virtual void drawPoint(const std::shared_ptr<VertexArray>& vertexArray) override;

   private:
    MetalContext& m_context;
    MTL::ClearColor m_color = MTL::ClearColor::Make(0, 0, 0, 1);

    NS::AutoreleasePool* m_pool;
    CA::MetalDrawable* m_drawable;
    MTL::RenderPassDescriptor* m_passDesc;
    MTL::CommandBuffer* m_buffer;

    MTL::CommandQueue* m_commandQueue;
	MTL::RenderCommandEncoder* m_encoder;

	std::shared_ptr<MetalFramebuffer> m_framebuffer;
};

}  // namespace Atlas