#include "atpch.h"
#include "MetalPipeline.h"

#include "MetalContext.h"
#include "metal-cpp/Metal.hpp"

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
		case Atlas::ShaderDataType::Uint:
			return MTL::VertexFormat::VertexFormatUInt;
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

MetalPipeline::MetalPipeline(const PipelineSpecification& specs) : m_name(specs.name) {
	AT_PROFILE_FUNCTION();

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

	m_pipelineState = MetalContext::getMTLDevice()->newRenderPipelineState(m_pipelineDescriptor, &error);

	AT_CORE_ASSERT(m_pipelineState, "Pipeline Error: {}", error->localizedDescription()->utf8String());
}

MetalPipeline::~MetalPipeline() {
	m_vertexDescriptor->release();
	m_pipelineDescriptor->release();
}

void MetalPipeline::attachFramebuffer(std::shared_ptr<Framebuffer> fb) {
	if (m_framebuffer == fb && m_pipelineState != nullptr) {
		return;
	}
	m_framebuffer = fb;

	std::shared_ptr<MetalFramebuffer> framebuffer = std::static_pointer_cast<MetalFramebuffer>(fb);
	m_pipelineDescriptor->setDepthAttachmentPixelFormat(framebuffer->getDepthTextureDescriptor()->pixelFormat());
	m_pipelineDescriptor->setSampleCount(framebuffer->getRasterSampleCount());

	for (size_t i = 0; i < framebuffer->getColorTextures().size(); i++) {
		MTL::RenderPipelineColorAttachmentDescriptor* colorAttach = m_pipelineDescriptor->colorAttachments()->object(i);
		colorAttach->setPixelFormat(framebuffer->getColorTextureDescriptor(i)->pixelFormat());
		colorAttach->setBlendingEnabled(true);
		colorAttach->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
		colorAttach->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
		colorAttach->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
		colorAttach->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
	}
	attachLayout();

	NS::Error* error = nullptr;
	m_pipelineState = MetalContext::getMTLDevice()->newRenderPipelineState(m_pipelineDescriptor, &error);

	AT_CORE_ASSERT(m_pipelineState, "Pipeline Error: {}", error->localizedDescription()->utf8String());
}

void MetalPipeline::setLayout(const BufferLayout& layout) {
	AT_PROFILE_FUNCTION();

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

void MetalPipeline::attachLayout() {
	AT_PROFILE_FUNCTION();

	m_pipelineDescriptor->setVertexDescriptor(m_vertexDescriptor);
}

}  // namespace Atlas