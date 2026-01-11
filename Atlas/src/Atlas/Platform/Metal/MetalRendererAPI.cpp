#include "MetalRendererAPI.h"
#include "atpch.h"

#include "MetalBuffer.h"
#include "MetalPipeline.h"
#include "MetalTexture.h"

#include <imgui/imgui.h>
#include "ImGuiMetalRenderer.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

MetalRendererAPI::MetalRendererAPI(MetalContext& context) : m_context(context) {
    AT_PROFILE_FUNCTION();

    m_commandQueue = context.getMTLDevice()->newCommandQueue();
}

MetalRendererAPI::~MetalRendererAPI() {
}

void MetalRendererAPI::setClearColor(const glm::vec4& color) {
    m_color = MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
}

void MetalRendererAPI::clear() {
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
    AT_PROFILE_FUNCTION();

    std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
    MTL::Buffer* indexBuffer = MIB->getBuffer();
    m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) {
    AT_PROFILE_FUNCTION();

    std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
    MTL::Buffer* indexBuffer = MIB->getBuffer();
    m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, count, MTL::IndexTypeUInt32, indexBuffer, 0);
}

void MetalRendererAPI::drawPoint(const std::shared_ptr<VertexArray>& vertexArray) {
    AT_PROFILE_FUNCTION();

    std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
    MTL::Buffer* indexBuffer = MIB->getBuffer();
    m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypePoint, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
}

void MetalRendererAPI::beginFrame() {
    AT_PROFILE_FUNCTION();

    m_pool = NS::AutoreleasePool::alloc()->init();
    
    {
        AT_PROFILE_SCOPE("Get new drawable and command buffer");

        m_drawable = m_context.getNextDrawable();
        m_passDesc = MTL::RenderPassDescriptor::alloc()->init();
        m_buffer = m_commandQueue->commandBuffer();
    }

    {
        AT_PROFILE_SCOPE("Create RenderPass ColorAttachment");

        auto descriptor = MTL::RenderPassColorAttachmentDescriptor::alloc()->init();
        descriptor->setTexture(m_context.getMSAATexture());
        descriptor->setResolveTexture(m_drawable->texture());
        descriptor->setLoadAction(MTL::LoadActionClear);
        descriptor->setStoreAction(MTL::StoreActionMultisampleResolve);
        descriptor->setClearColor(m_color);
        m_passDesc->colorAttachments()->setObject(descriptor, 0);
        descriptor->release();
    }

    {
        AT_PROFILE_SCOPE("Create RenderPass DepthAttachment");
        auto depthAttachment = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
        depthAttachment->setTexture(m_context.getDepthTexture());
        depthAttachment->setLoadAction(MTL::LoadActionClear);
        depthAttachment->setStoreAction(MTL::StoreActionDontCare);
        m_passDesc->setDepthAttachment(depthAttachment);
        depthAttachment->release();
    }

    {
        AT_PROFILE_SCOPE("Set RenderCommandEncoder");

        m_encoder = m_buffer->renderCommandEncoder(m_passDesc);
    }

    clear();
}

void MetalRendererAPI::bindPipeline(const Pipeline& pipeline, const UniformBuffer& uBuffer) {
    AT_PROFILE_FUNCTION();

    const MetalPipeline& p = static_cast<const MetalPipeline&>(pipeline);
    m_encoder->setRenderPipelineState(p.getMTLPSO());
    m_encoder->setVertexBuffer(static_cast<MTL::Buffer*>(uBuffer.getNativeBuffer()), 0, uBuffer.getIndex());
    m_encoder->setFragmentBuffer(static_cast<MTL::Buffer*>(uBuffer.getNativeBuffer()), 0, uBuffer.getIndex());
}

void MetalRendererAPI::bindTexture(const Pipeline& pipeline, const Texture& texture, uint32_t index) {
    AT_PROFILE_FUNCTION();

    const MetalTexture& t = static_cast<const MetalTexture&>(texture);
    m_encoder->setFragmentTexture(t.getMTLTexture(), index);
}

void MetalRendererAPI::bindVertexArray(const VertexArray& array) {
    AT_PROFILE_FUNCTION();

    int index = 0;
    for (auto buffer : array.getVertexBuffers()) {
        bindVertexBuffer(*buffer, index++);
    }
}

void MetalRendererAPI::bindVertexBuffer(const VertexBuffer& buffer, int index) {
    AT_PROFILE_FUNCTION();

    const MetalVertexBuffer& mBuf = static_cast<const MetalVertexBuffer&>(buffer);
    m_encoder->setVertexBuffer((MTL::Buffer*)mBuf.getNativeBuffer(), 0, index);
}

// TODO: switch to batch system
void MetalRendererAPI::endFrame() {
    AT_PROFILE_FUNCTION();

    commit();

    m_passDesc->release();
    m_pool->release();
}

void MetalRendererAPI::beginImGui() {
    AT_PROFILE_FUNCTION();

    ImGui_ImplMetal_NewFrame(m_passDesc);
}

void MetalRendererAPI::drawImGui() {
    AT_PROFILE_FUNCTION();

    ImGui_ImplMetal_RenderDrawData(
        ImGui::GetDrawData(),
        m_buffer,
        m_encoder);
}

void MetalRendererAPI::commit() {
    AT_PROFILE_FUNCTION();
    
    // Present the drawable
    m_encoder->endEncoding();

    m_buffer->presentDrawable((MTL::Drawable*)m_drawable);
    m_buffer->commit();
}

}  // namespace Atlas