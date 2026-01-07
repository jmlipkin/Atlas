
void MetalContext::beginFrame() {
    m_pool = NS::AutoreleasePool::alloc()->init();
    s_drawable = m_layer->nextDrawable();
    if (!s_drawable) {
        AT_WARN("CA::MetalDrawable* not drawable (MetalContext)");
        return;
    }
}

void MacOSWindow::onUpdate() {
    glfwPollEvents();
    m_data.context->swapBuffers();
}

void MetalRendererAPI::setClearColor(const glm::vec4& color) {
    MTL::ClearColor clr = MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
    m_passDesc->colorAttachments()->object(0)->setClearColor(clr);
}

void MetalRendererAPI::clear() {
    // Ensure color attachment uses the drawable's texture
    MTL::RenderPassColorAttachmentDescriptor* attachment =
        m_passDesc->colorAttachments()->object(0);
    attachment->setTexture(m_drawable->texture());
    attachment->setLoadAction(MTL::LoadActionClear);
    attachment->setStoreAction(MTL::StoreActionStore);

    // Encode
    m_commandBuffer = MetalContext::getCommandQueue()->commandBuffer();
    s_commandEncoder = m_commandBuffer->renderCommandEncoder(m_passDesc);
}

// Renderer::beginScene();

void MetalShader::bind() const {
    MTL::RenderCommandEncoder* encoder = MetalRendererAPI::getMTLEncoder();
    encoder->setRenderPipelineState(m_pipelineState);
}

void MetalVertexArray::bind() const {
    
    for (auto buffer : m_vertexBuffers) {
        MetalRendererAPI::getMTLEncoder()->setVertexBuffer(buffer, 0, index);
    }
}

void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
    std::shared_ptr<MetalIndexBuffer> MIB = std::static_pointer_cast<MetalIndexBuffer>(vertexArray->getIndexBuffer());
    MTL::Buffer* indexBuffer = MIB->getBuffer();
    s_commandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, vertexArray->getIndexBuffer()->getCount(), MTL::IndexTypeUInt32, indexBuffer, 0);
}

// Renderer::endScene();

void MetalRendererAPI::commit() {
    // Present the drawable
    s_commandEncoder->endEncoding();

    m_commandBuffer->presentDrawable((MTL::Drawable*)MetalContext::getMTLDrawable());
    m_commandBuffer->commit();
}

void MetalContext::endFrame() {
    m_pool->release();
}