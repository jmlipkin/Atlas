#include "MetalPipeline.h"
#include "atpch.h"

#include "MetalContext.h"

// TEMP
#include "Atlas/Renderer/RenderCommand.h"

namespace Atlas {

static MTL::VertexFormat shaderDataTypeToMTLFormat(ShaderDataType type) {
    switch (type) {
        case Atlas::ShaderDataType::None:
            return MTL::VertexFormat::VertexFormatInvalid;
        case Atlas::ShaderDataType::Float:
            return MTL::VertexFormat::VertexFormatFloat;
        case Atlas::ShaderDataType::Float2:
            return MTL::VertexFormat::VertexFormatFloat2;
        case Atlas::ShaderDataType::Float3:
            return MTL::VertexFormat::VertexFormatFloat3;
        case Atlas::ShaderDataType::Float4:
            return MTL::VertexFormat::VertexFormatFloat4;
        case Atlas::ShaderDataType::Mat3:
            return MTL::VertexFormat::VertexFormatInvalid;
        case Atlas::ShaderDataType::Mat4:
            return MTL::VertexFormat::VertexFormatInvalid;
        case Atlas::ShaderDataType::Int:
            return MTL::VertexFormat::VertexFormatInt;
        case Atlas::ShaderDataType::Int2:
            return MTL::VertexFormat::VertexFormatInt2;
        case Atlas::ShaderDataType::Int3:
            return MTL::VertexFormat::VertexFormatInt3;
        case Atlas::ShaderDataType::Int4:
            return MTL::VertexFormat::VertexFormatInt4;
        case Atlas::ShaderDataType::Bool:
            return MTL::VertexFormat::VertexFormatUInt;
    }

    AT_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return MTL::VertexFormat::VertexFormatInvalid;
}

MetalPipeline::MetalPipeline(const PipelineSpecification& specs) {
    NS::Error* error = nullptr;
    m_pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    m_pipelineDescriptor->setLabel(NS::String::string(specs.name.c_str(), NS::ASCIIStringEncoding));

    MTL::Function* vs = (MTL::Function*)specs.shader->getVertexShader();
    MTL::Function* fs = (MTL::Function*)specs.shader->getFragmentShader();

    m_pipelineDescriptor->setVertexFunction(vs);
    m_pipelineDescriptor->setFragmentFunction(fs);

    m_vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    setLayout(specs.layout);
    attachLayout();

    MTL::RenderPipelineColorAttachmentDescriptor* colorAttach = m_pipelineDescriptor->colorAttachments()->object(0);

    // TEMPORARY
    colorAttach->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    colorAttach->setBlendingEnabled(true);
    colorAttach->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
    colorAttach->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setRgbBlendOperation(MTL::BlendOperationAdd);
    colorAttach->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
    colorAttach->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
    colorAttach->setAlphaBlendOperation(MTL::BlendOperationAdd);

    m_pipelineState = MetalContext::getMTLDevice()->newRenderPipelineState(m_pipelineDescriptor, &error);

    AT_CORE_ASSERT(m_pipelineState, error->localizedDescription()->utf8String());
}

MetalPipeline::~MetalPipeline() {
    m_vertexDescriptor->release();
    m_pipelineDescriptor->release();
}

void MetalPipeline::setLayout(const BufferLayout& layout) {
    AT_CORE_ASSERT(layout.getElements().size(), "Layout is empty!");

    uint32_t index = 0;
    for (const auto& element : layout) {
        MTL::VertexAttributeDescriptor* attribute = m_vertexDescriptor->attributes()->object(index);
        attribute->setFormat(shaderDataTypeToMTLFormat(element.type));
        attribute->setOffset(element.offset);
        attribute->setBufferIndex(0);

        index++;
    }
    MTL::VertexBufferLayoutDescriptor* bufferLayout = m_vertexDescriptor->layouts()->object(0);
    bufferLayout->setStride(layout.getStride());
    bufferLayout->setStepFunction(MTL::VertexStepFunctionPerVertex);
}

void MetalPipeline::setBool(const std::string& name, const bool value) {
}

void MetalPipeline::setInt(const std::string& name, const int value) {
    
}

void MetalPipeline::setFloat(const std::string& name, const float value) {
    
}

void MetalPipeline::setFloat2(const std::string& name, const glm::vec2& value) {
    
}

void MetalPipeline::setFloat3(const std::string& name, const glm::vec3& value) {
    
}

void MetalPipeline::setFloat4(const std::string& name, const glm::vec4& value) {
    RenderCommand::setFloat4(name, value);
}

void MetalPipeline::setMat3(const std::string& name, const glm::mat3& value) {
    
}

void MetalPipeline::setMat4(const std::string& name, const glm::mat4& value) {
    
}


void MetalPipeline::attachLayout() {
    m_pipelineDescriptor->setVertexDescriptor(m_vertexDescriptor);
}

}  // namespace Atlas