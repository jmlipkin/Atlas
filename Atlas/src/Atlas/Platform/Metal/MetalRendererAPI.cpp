#include "MetalRendererAPI.h"
#include "atpch.h"

#include "MetalBuffer.h"
#include "MetalPipeline.h"

#include <imgui/imgui.h>
#include "ImGuiMetalRenderer.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

MetalRendererAPI::MetalRendererAPI(MetalContext& context) : m_context(context) {
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
    std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
    MTL::Buffer* indexBuffer = MIB->getBuffer();
    m_encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
}

void MetalRendererAPI::beginFrame() {
    m_pool = NS::AutoreleasePool::alloc()->init();
    m_drawable = m_context.getNextDrawable();
    m_passDesc = MTL::RenderPassDescriptor::alloc()->init();
    m_buffer = m_commandQueue->commandBuffer();

    auto descriptor = MTL::RenderPassColorAttachmentDescriptor::alloc()->init();
    descriptor->setTexture(m_drawable->texture());
    descriptor->setLoadAction(MTL::LoadActionClear);
    descriptor->setStoreAction(MTL::StoreActionStore);
    descriptor->setClearColor(m_color);
    m_passDesc->colorAttachments()->setObject(descriptor, 0);
    descriptor->release();

    m_encoder = m_buffer->renderCommandEncoder(m_passDesc);

    clear();
}

void MetalRendererAPI::bindPipeline(const Pipeline& pipeline) {
    const MetalPipeline& p = static_cast<const MetalPipeline&>(pipeline);
    m_encoder->setRenderPipelineState(p.getMTLPSO());
}

void MetalRendererAPI::bindVertexArray(const VertexArray& array) {
    int index = 0;
    for (auto buffer : array.getVertexBuffers()) {
        bindVertexBuffer(*buffer, index++);
    }
}

void MetalRendererAPI::bindVertexBuffer(const VertexBuffer& buffer, int index) {
    const MetalVertexBuffer& mBuf = static_cast<const MetalVertexBuffer&>(buffer);
    m_encoder->setVertexBuffer((MTL::Buffer*)mBuf.getNativeBuffer(), 0, index);
}

// TODO: switch to batch system
void MetalRendererAPI::endFrame() {
    commit();

    m_passDesc->release();
    m_pool->release();
}

void MetalRendererAPI::beginImGui() {
    ImGui_ImplMetal_NewFrame(m_passDesc);
}

void MetalRendererAPI::drawImGui() {
    ImGui_ImplMetal_RenderDrawData(
        ImGui::GetDrawData(),
        m_buffer,
        m_encoder);
}

void MetalRendererAPI::commit() {
    // Present the drawable
    m_encoder->endEncoding();

    m_buffer->presentDrawable((MTL::Drawable*)m_drawable);
    m_buffer->commit();
}

}  // namespace Atlas