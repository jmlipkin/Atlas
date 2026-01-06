#include "atpch.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>
#include "MetalBuffer.h"
#include "MetalShader.h"

namespace Atlas {

MetalRendererAPI::MetalRendererAPI(MetalContext& context) : m_context(context) {
    m_commandQueue = context.getMTLDevice()->newCommandQueue();
}

MetalRendererAPI::~MetalRendererAPI() {
    // m_passDesc->release();
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
    // MetalContext::setEncoder(encoder);

    clear();
}

void MetalRendererAPI::bindShader(const Shader& shader) {
    const MetalShader& s = static_cast<const MetalShader&>(shader);
    m_encoder->setRenderPipelineState(s.getMTLPSO());
}

void MetalRendererAPI::bindVertexArray(const VertexArray& array) {
    int index = 0;
    for (auto buffer : array.getVertexBuffers()) {
        bindVertexBuffer(*buffer, index++);
    }
}

void MetalRendererAPI::bindVertexBuffer(const VertexBuffer& buffer, int index) {
    m_encoder->setVertexBuffer(static_cast<const MetalVertexBuffer&>(buffer).getNativeBuffer(), 0, index);
}

// TODO: switch to batch system
void MetalRendererAPI::endFrame() {
    commit();

    m_passDesc->release();
    m_pool->release();
}

void MetalRendererAPI::commit() {
    // Present the drawable
    m_encoder->endEncoding();

    m_buffer->presentDrawable((MTL::Drawable*)m_drawable);
    m_buffer->commit();
}

}  // namespace Atlas