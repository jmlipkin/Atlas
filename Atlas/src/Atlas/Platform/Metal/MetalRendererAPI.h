#pragma once

#include "Atlas/Renderer/RendererAPI.h"

#include "Atlas/Platform/Metal/MetalContext.h"
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

    // ImGui functions implemented in MetalRendererAPI.mm
    virtual void beginImGui() override;
    virtual void drawImGui() override;

    void bindPipeline(const Pipeline& shader, const UniformBuffer& uBuffer) override;
    void bindTexture(const Pipeline& shader, const Texture& texture, uint32_t index) override;
    void bindVertexArray(const VertexArray& array) override;
    void bindVertexBuffer(const VertexBuffer& buffer, int index) override;

    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) override;
    virtual void drawPoint(const std::shared_ptr<VertexArray>& vertexArray) override;
    virtual void commit() override;

   private:
    MetalContext& m_context;
    MTL::ClearColor m_color;

    NS::AutoreleasePool* m_pool;
    CA::MetalDrawable* m_drawable;
    MTL::RenderPassDescriptor* m_passDesc;
    MTL::CommandBuffer* m_buffer;

    MTL::CommandQueue* m_commandQueue;
    MTL::RenderCommandEncoder* m_encoder;
};

}  // namespace Atlas