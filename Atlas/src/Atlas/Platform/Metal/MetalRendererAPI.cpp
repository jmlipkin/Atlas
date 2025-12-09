#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

#include "atpch.h"

namespace Atlas {
MetalRendererAPI::MetalRendererAPI() : m_pDevice(*MetalContext::getMTLDevice()) {
    AT_CORE_INFO("MTLDevice name: {} (from MetalRendererAPI)", m_pDevice.name()->cString(NS::ASCIIStringEncoding));
    m_renderPDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    m_commandQueue = m_pDevice.newCommandQueue();

    auto descriptor = MTL::RenderPassColorAttachmentDescriptor::alloc()->init();
    descriptor->setLoadAction(MTL::LoadActionClear);
    descriptor->setStoreAction(MTL::StoreActionStore);
    m_renderPDescriptor->colorAttachments()->setObject(descriptor, 0);
    descriptor->release();
}

MetalRendererAPI::~MetalRendererAPI() {
    m_renderPDescriptor->release();
}

void MetalRendererAPI::setClearColor(const glm::vec4& color) {
    MTL::ClearColor clr = MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
    m_renderPDescriptor->colorAttachments()->object(0)->setClearColor(clr);

    // AT_CORE_WARN("MetalRendererAPI setClearColor(const glm::vec4& color) not implemented");
}

void MetalRendererAPI::clear() {
    // NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    m_drawable = MetalContext::getMTLLayer()->nextDrawable();
    if (!m_drawable) {
        AT_WARN("CA::MetalDrawable* Not drawable (MetalRendererAPI)");
        // pool->release();
        return;
    }
    // AT_CORE_WARN("MetalRendererAPI clear() not implemented");
    // renderPDescriptor->colorAttachments()->object(0);

    // Ensure color attachment uses the drawable's texture
    MTL::RenderPassColorAttachmentDescriptor* attachment =
        m_renderPDescriptor->colorAttachments()->object(0);
    attachment->setTexture(m_drawable->texture());
    attachment->setLoadAction(MTL::LoadActionClear);
    attachment->setStoreAction(MTL::StoreActionStore);

    // Encode
    m_commandBuffer = m_commandQueue->commandBuffer();
    MTL::RenderCommandEncoder* encoder =
        m_commandBuffer->renderCommandEncoder(m_renderPDescriptor);

    encoder->endEncoding();
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
    AT_CORE_WARN("MetalRendererAPI drawIndexed() not implemented");
}

void MetalRendererAPI::commit() {
    // Present the drawable
    m_commandBuffer->presentDrawable((MTL::Drawable*)m_drawable);
    m_commandBuffer->commit();
}

}  // namespace Atlas