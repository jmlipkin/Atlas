#pragma once

#include "Atlas/Renderer/Framebuffer.h"

#include <metal-cpp/Metal.hpp>
#include <vector>

namespace Atlas {

class MetalFramebuffer : public Framebuffer {
  public:
	MetalFramebuffer(const FramebufferSpecifications& specs);
	virtual ~MetalFramebuffer() override;

	virtual void* getPassDescriptor(void* drawable) override;

	virtual void* getDepthStencilState() const override { return m_depthStencilState; }
	virtual void* setDepthStencilState() override;

	virtual void onResize(uint32_t width, uint32_t height) override;
	virtual void invalidate() override;

	virtual uint32_t getWidth() const override { return m_specs.width; }
	virtual uint32_t getHeight() const override { return m_specs.height; }

	virtual bool isSwapChainTarget() const override { return m_specs.isSwapChainTarget; }
	virtual bool isDepthEnabled() const override { return m_specs.isDepthEnabled; }
	virtual FramebufferPixelFormat getDepthAttachmentPixelFormat() const override { return toAtlasPixelFormat(m_depthDescriptor->pixelFormat()); }
	virtual FramebufferPixelFormat getColorAttachmentPixelFormat(int index) const override { return toAtlasPixelFormat(m_colorDescriptors[index]->pixelFormat()); }
	virtual int getRasterSampleCount() const override { return m_specs.samples; }

	std::vector<MTL::Texture*> getColorTextures() { return m_colorTextures; }
	std::vector<MTL::Texture*> getMSAATextures() { return m_msaaTextures; }
	MTL::Texture* getColorTexture(int index) { return m_colorTextures[index]; }
	MTL::Texture* getMSAATexture(int index) { return m_msaaTextures[index]; }
	MTL::Texture* getDepthTexture() { return m_depthTexture; }

	std::vector<MTL::TextureDescriptor*> getColorTextureDescriptors() { return m_colorDescriptors; }
	std::vector<MTL::TextureDescriptor*> getMSAATextureDescriptors() { return m_msaaDescriptors; }
	MTL::TextureDescriptor* getColorTextureDescriptor(int index) { return m_colorDescriptors[index]; }
	MTL::TextureDescriptor* getMSAATextureDescriptor(int index) { return m_msaaDescriptors[index]; }
	MTL::TextureDescriptor* getDepthTextureDescriptor() { return m_depthDescriptor; }

	static MTL::PixelFormat toMTLPixelFormat(FramebufferPixelFormat fbFormat);
	static FramebufferPixelFormat toAtlasPixelFormat(MTL::PixelFormat fbFormat);

  private:
	virtual void createColorTexture(int index) override;
	virtual void createMSAATexture(int index) override;
	virtual void createDepthTexture() override;

	void createTextures();
	void releaseTextures();

  private:
	FramebufferSpecifications m_specs;
	MTL::RenderPassDescriptor* m_passDesc;

	std::vector<MTL::Texture*> m_colorTextures;
	std::vector<MTL::Texture*> m_msaaTextures;
	MTL::Texture* m_depthTexture;

	std::vector<MTL::TextureDescriptor*> m_colorDescriptors;
	std::vector<MTL::TextureDescriptor*> m_msaaDescriptors;
	MTL::TextureDescriptor* m_depthDescriptor;

	MTL::DepthStencilState* m_depthStencilState;
};

}  // namespace Atlas