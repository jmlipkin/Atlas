#include "MetalShader.h"

#include "atpch.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

namespace Atlas {

MetalShader::MetalShader(const std::string& filepath) {
    MTL::Library* library = MetalContext::setNewMTLLibrary(filepath);

    auto* vs = MetalContext::getMTLLibrary()->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
    auto* fs = MetalContext::getMTLLibrary()->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));

    MTL::RenderPipelineDescriptor* renderPipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();

    renderPipeDesc->setLabel(NS::String::string("Test!", NS::ASCIIStringEncoding));
    renderPipeDesc->setVertexFunction(vs);
    renderPipeDesc->setFragmentFunction(fs);

    MTL::RenderPipelineColorAttachmentDescriptor* colorAttach = renderPipeDesc->colorAttachments()->object(0);

    // TEMPORARY
    colorAttach->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    colorAttach->setBlendingEnabled(true);
    colorAttach->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
    colorAttach->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setRgbBlendOperation(MTL::BlendOperationAdd);
    colorAttach->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
    colorAttach->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setAlphaBlendOperation(MTL::BlendOperationAdd);

    NS::Error* error = nullptr;
    m_pipelineState = MetalContext::getMTLDevice()->newRenderPipelineState(renderPipeDesc, &error);

    AT_CORE_ASSERT(m_pipelineState, error->localizedDescription()->utf8String());
    renderPipeDesc->release();
}

MetalShader::MetalShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc) {

    auto* vs = MetalContext::getMTLLibrary()->newFunction(NS::String::string(vertexSrc.c_str(), NS::UTF8StringEncoding));
    auto* fs = MetalContext::getMTLLibrary()->newFunction(NS::String::string(fragSrc.c_str(), NS::UTF8StringEncoding));

    MTL::RenderPipelineDescriptor* renderPipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();

    renderPipeDesc->setLabel(NS::String::string(name.c_str(), NS::ASCIIStringEncoding));
    renderPipeDesc->setVertexFunction(vs);
    renderPipeDesc->setFragmentFunction(fs);

    MTL::RenderPipelineColorAttachmentDescriptor* colorAttach = renderPipeDesc->colorAttachments()->object(0);

    // TEMPORARY
    colorAttach->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    colorAttach->setBlendingEnabled(true);
    colorAttach->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
    colorAttach->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setRgbBlendOperation(MTL::BlendOperationAdd);
    colorAttach->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
    colorAttach->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setAlphaBlendOperation(MTL::BlendOperationAdd);

    NS::Error* error = nullptr;
    m_pipelineState = MetalContext::getMTLDevice()->newRenderPipelineState(renderPipeDesc, &error);

    AT_CORE_ASSERT(m_pipelineState, error->localizedDescription()->utf8String());
    renderPipeDesc->release();
}

MetalShader::~MetalShader() {
    m_pipelineState->release();
}

// void MetalShader::bind() const {
//     // MTL::RenderCommandEncoder* encoder = MetalRendererAPI::getEncoder();
//     // encoder->setRenderPipelineState(m_pipelineState);

//     AT_CORE_WARN("MetalShader bind not yet implemented");
// }

// void MetalShader::unbind() const {
//     AT_CORE_WARN("MetalShader unbind not yet implemented");
// }

}  // namespace Atlas