#include "atpch.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>
#include "MetalBuffer.h"

namespace Atlas {

MetalRendererAPI::MetalRendererAPI(std::shared_ptr<Window> window) {
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
    MetalContext::getEncoder()->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
}

void MetalRendererAPI::beginScene() {
    auto* passDesc = MetalContext::getPassDesc();
    auto* drawable = MetalContext::getMTLDrawable();

    auto descriptor = MTL::RenderPassColorAttachmentDescriptor::alloc()->init();
    descriptor->setTexture(drawable->texture());
    descriptor->setLoadAction(MTL::LoadActionClear);
    descriptor->setStoreAction(MTL::StoreActionStore);
    descriptor->setClearColor(m_color);
    passDesc->colorAttachments()->setObject(descriptor, 0);
    descriptor->release();

    auto* encoder = MetalContext::getCommandBuffer()->renderCommandEncoder(passDesc);
    MetalContext::setEncoder(encoder);
}

void MetalRendererAPI::commit() {
    // Present the drawable
    MetalContext::getEncoder()->endEncoding();

    MetalContext::getCommandBuffer()->presentDrawable((MTL::Drawable*)MetalContext::getMTLDrawable());
    MetalContext::getCommandBuffer()->commit();
}

void MetalRendererAPI::onEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(MetalRendererAPI::onWindowResize));
}

bool MetalRendererAPI::onWindowResize(WindowResizeEvent& e) {
    // m_layer->setDrawableSize(CGSizeMake(e.getWidth(), e.getHeight()));

    return false;
}

}  // namespace Atlas