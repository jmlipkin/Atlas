#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

#include "atpch.h"

#include <Atlas/Events/ApplicationEvent.h>

namespace Atlas {
MetalRendererAPI::MetalRendererAPI() : m_MTLData(MetalContext::getMTLData()) {
    AT_CORE_INFO("MTLDevice name: {} (from MetalRendererAPI)", m_MTLData->device->name()->cString(NS::ASCIIStringEncoding));
    m_MTLData->passDesc = MTL::RenderPassDescriptor::alloc()->init();

    auto descriptor = MTL::RenderPassColorAttachmentDescriptor::alloc()->init();
    descriptor->setLoadAction(MTL::LoadActionClear);
    descriptor->setStoreAction(MTL::StoreActionStore);
    m_MTLData->passDesc->colorAttachments()->setObject(descriptor, 0);
    descriptor->release();
}

MetalRendererAPI::~MetalRendererAPI() {
    m_MTLData->passDesc->release();
}

void MetalRendererAPI::setClearColor(const glm::vec4& color) {
    MTL::ClearColor clr = MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
    m_MTLData->passDesc->colorAttachments()->object(0)->setClearColor(clr);

    // AT_CORE_WARN("MetalRendererAPI setClearColor(const glm::vec4& color) not implemented");
}

void MetalRendererAPI::clear() {
    // NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    m_MTLData->currentDrawable = m_MTLData->layer->nextDrawable();
    CA::MetalDrawable* drawable = m_MTLData->currentDrawable;
    if (!drawable) {
        AT_WARN("CA::MetalDrawable* Not drawable (MetalRendererAPI)");
        // pool->release();
        return;
    }
    // AT_CORE_WARN("MetalRendererAPI clear() not implemented");
    // renderPDescriptor->colorAttachments()->object(0);

    // Ensure color attachment uses the drawable's texture
    MTL::RenderPassColorAttachmentDescriptor* attachment =
        m_MTLData->passDesc->colorAttachments()->object(0);
    attachment->setTexture(drawable->texture());
    attachment->setLoadAction(MTL::LoadActionClear);
    attachment->setStoreAction(MTL::StoreActionStore);

    // Encode
    m_MTLData->commandBuffer = m_MTLData->commandQueue->commandBuffer();
    MTL::RenderCommandEncoder* encoder =
        m_MTLData->commandBuffer->renderCommandEncoder(m_MTLData->passDesc);

    encoder->endEncoding();
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
    AT_CORE_WARN("MetalRendererAPI drawIndexed() not implemented");
}

void MetalRendererAPI::commit() {
    // Present the drawable
    m_MTLData->commandBuffer->presentDrawable((MTL::Drawable*)m_MTLData->currentDrawable);
    m_MTLData->commandBuffer->commit();
}

void MetalRendererAPI::onEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(MetalRendererAPI::onWindowResize));
}

bool MetalRendererAPI::onWindowResize(WindowResizeEvent& e) {
    CA::MetalLayer* layer = m_MTLData->layer;
    layer->setDrawableSize(CGSizeMake(e.getWidth(), e.getHeight()));

    return false;
}

}  // namespace Atlas