#include "Atlas/Core/Application.h"
#include "Atlas/Renderer/Framebuffer.h"
#include "GLFW/glfw3.h"
#include "atpch.h"
#include "MetalContext.h"
#include "MetalFramebuffer.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

MetalFramebuffer::MetalFramebuffer(const FramebufferSpecifications& specs) : m_specs(specs) {
	m_passDesc = MTL::RenderPassDescriptor::alloc()->init();
	createTextures();
}

MetalFramebuffer::~MetalFramebuffer() {
	m_passDesc->release();
}

void* MetalFramebuffer::getPassDescriptor(void* drawable) {
	CA::MetalDrawable* mtlDrawable = (CA::MetalDrawable*)drawable;

	for (size_t i = 0; i < m_colorDescriptors.size(); i++) {
		glm::vec4 color = m_specs.colorAttachments[i].clearColor;
		MTL::ClearColor clear = MTL::ClearColor::Make(color.r, color.g, color.b, color.a);
		MTL::Texture* colorTexture = (m_specs.isSwapChainTarget) ? mtlDrawable->texture() : m_colorTextures[i];

		MTL::RenderPassColorAttachmentDescriptor* colorDesc = m_passDesc->colorAttachments()->object(i);

		if (m_specs.samples > 1) {
			AT_CORE_ASSERT(colorTexture, "Swap chain target must have a drawable texture!");
			colorDesc->setTexture(m_msaaTextures[i]);
			colorDesc->setResolveTexture(colorTexture);
			colorDesc->setStoreAction(MTL::StoreActionMultisampleResolve);
		} else {
			AT_CORE_ASSERT(colorTexture, "Swap chain target must have a drawable texture!");
			colorDesc->setTexture(colorTexture);
			colorDesc->setStoreAction(MTL::StoreActionStore);
		}

		colorDesc->setLoadAction(MTL::LoadActionClear);
		colorDesc->setClearColor(clear);
	}

	if (m_specs.isDepthEnabled) {
		MTL::RenderPassDepthAttachmentDescriptor* depthDesc = m_passDesc->depthAttachment();
		depthDesc->setTexture(m_depthTexture);
		depthDesc->setLoadAction(MTL::LoadActionClear);
		depthDesc->setStoreAction(MTL::StoreActionDontCare);
		depthDesc->setClearDepth(1.0f);
	}

	return m_passDesc;
}

void MetalFramebuffer::onResize(uint32_t width, uint32_t height) {
	GLFWwindow* window = (GLFWwindow*)Application::get().getWindow().getNativeWindow();
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	m_specs.width = w;
	m_specs.height = h;

	invalidate();
}

void MetalFramebuffer::invalidate() {
	releaseTextures();
	createTextures();
}

void MetalFramebuffer::createColorTexture(int index) {
	FramebufferPixelFormat format = m_specs.colorAttachments[index].format;
	MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
	desc->setTextureType(MTL::TextureType2D);
	desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
	desc->setWidth(m_specs.width);
	desc->setHeight(m_specs.height);
	desc->setPixelFormat(toMTLPixelFormat(format));
	desc->setSampleCount(1);

	MTL::Texture* texture = MetalContext::getMTLDevice()->newTexture(desc);
	m_colorTextures.push_back(texture);
	m_colorDescriptors.push_back(desc);
}

void MetalFramebuffer::createMSAATexture(int index) {
	FramebufferPixelFormat format = m_specs.colorAttachments[index].format;
	MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
	desc->setTextureType(MTL::TextureType2DMultisample);
	desc->setUsage(MTL::TextureUsageRenderTarget);
	desc->setWidth(m_specs.width);
	desc->setHeight(m_specs.height);
	desc->setPixelFormat(toMTLPixelFormat(format));
	desc->setSampleCount(m_specs.samples);

	MTL::Texture* texture = Atlas::MetalContext::getMTLDevice()->newTexture(desc);
	m_msaaTextures.push_back(texture);
	m_msaaDescriptors.push_back(desc);
}

void MetalFramebuffer::createDepthTexture() {
	FramebufferPixelFormat format = m_specs.depthAttachment.format;
	m_depthDescriptor = MTL::TextureDescriptor::alloc()->init();
	if (m_specs.samples > 1)
		m_depthDescriptor->setTextureType(MTL::TextureType2DMultisample);
	else
		m_depthDescriptor->setTextureType(MTL::TextureType2D);
	m_depthDescriptor->setSampleCount(m_specs.samples);
	m_depthDescriptor->setUsage(MTL::TextureUsageRenderTarget);
	m_depthDescriptor->setWidth(m_specs.width);
	m_depthDescriptor->setHeight(m_specs.height);
	m_depthDescriptor->setPixelFormat(toMTLPixelFormat(format));
	m_depthDescriptor->setStorageMode(MTL::StorageModePrivate);

	MTL::Texture* texture = Atlas::MetalContext::getMTLDevice()->newTexture(m_depthDescriptor);
	m_depthTexture = texture;
}

void MetalFramebuffer::createTextures() {
	AT_CORE_ASSERT(m_specs.colorAttachments.size() == 1, "Atlas currently only supports 1 color attachment");

	for (int i = 0; i < m_specs.colorAttachments.size(); i++) {
		createColorTexture(i);
		if (m_specs.samples > 1) {
			createMSAATexture(i);
		}
	}

	if (m_specs.isDepthEnabled) {
		createDepthTexture();
	}
}

void MetalFramebuffer::releaseTextures() {
	for (auto* tex : m_colorTextures) {
		tex->release();
	}
	m_colorTextures.clear();

	for (auto* tex : m_msaaTextures) {
		tex->release();
	}
	m_msaaTextures.clear();

	if (m_depthTexture) {
		m_depthTexture->release();
		m_depthTexture = nullptr;
	}

	for (auto* desc : m_colorDescriptors)
		desc->release();
	m_colorDescriptors.clear();

	for (auto* desc : m_msaaDescriptors)
		desc->release();
	m_msaaDescriptors.clear();

	if (m_depthDescriptor) {
		m_depthDescriptor->release();
		m_depthDescriptor = nullptr;
	}
}

MTL::PixelFormat MetalFramebuffer::toMTLPixelFormat(FramebufferPixelFormat fbFormat) {
	switch (fbFormat) {
		case FramebufferPixelFormat::NONE:
			AT_CORE_ASSERT(false, "Framebuffer pixel format cannot be none!");
		case FramebufferPixelFormat::RGBA8:
			return MTL::PixelFormatRGBA8Unorm;
		case FramebufferPixelFormat::DEPTH32FLOAT:
			return MTL::PixelFormatDepth32Float;
		case FramebufferPixelFormat::DEPTH24STENCIL8:
			return MTL::PixelFormatDepth24Unorm_Stencil8;
	}
}

FramebufferPixelFormat MetalFramebuffer::toAtlasPixelFormat(MTL::PixelFormat fbFormat) {
	switch (fbFormat) {
		case MTL::PixelFormatRGBA8Unorm:
			return FramebufferPixelFormat::RGBA8;
		case MTL::PixelFormatDepth32Float:
			return FramebufferPixelFormat::DEPTH32FLOAT;
		case MTL::PixelFormatDepth24Unorm_Stencil8:
			return FramebufferPixelFormat::DEPTH24STENCIL8;
		default: {
			AT_CORE_ASSERT(false, "Unsupported pixel format!");
			return FramebufferPixelFormat::NONE;
		}
	}
}

}  // namespace Atlas