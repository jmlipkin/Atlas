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
    descriptor->setTexture(m_context.getMSAATexture());
    descriptor->setResolveTexture(m_drawable->texture());
    descriptor->setLoadAction(MTL::LoadActionClear);
    descriptor->setStoreAction(MTL::StoreActionMultisampleResolve);
    descriptor->setClearColor(m_color);
    m_passDesc->colorAttachments()->setObject(descriptor, 0);
    descriptor->release();

    auto depthAttachment = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
    depthAttachment->setTexture(m_context.getDepthTexture());
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);
    m_passDesc->setDepthAttachment(depthAttachment);
    depthAttachment->release();

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

void MetalRendererAPI::setBool(const std::string& name, const bool value) {
}

void MetalRendererAPI::setInt(const std::string& name, const int value) {

}

void MetalRendererAPI::setFloat(const std::string& name, const float value) {

}

void MetalRendererAPI::setFloat2(const std::string& name, const glm::vec2& value) {

}

void MetalRendererAPI::setFloat3(const std::string& name, const glm::vec3& value) {

}

// TEMPORARY
void MetalRendererAPI::setFloat4(const std::string& name, const glm::vec4& value) {
    struct Uniforms {
        glm::vec4 color;
    };
    MTL::Buffer* uniformBuffer = MetalContext::getMTLDevice()->newBuffer(sizeof(Uniforms), MTL::ResourceStorageModeManaged);
    Uniforms* u = reinterpret_cast<Uniforms*>(uniformBuffer->contents());
    u->color = value;

    m_encoder->setVertexBuffer(uniformBuffer, 0, 1);
    m_encoder->setFragmentBuffer(uniformBuffer, 0, 1);
}

void MetalRendererAPI::setMat3(const std::string& name, const glm::mat3& value) {

}

void MetalRendererAPI::setMat4(const std::string& name, const glm::mat4& value) {

}


}  // namespace Atlas