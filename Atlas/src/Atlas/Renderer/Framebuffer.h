#pragma once

namespace Atlas {

enum class FramebufferPixelFormat {
	NONE = 0,
	// Color
	RGBA8,
	// Depth
	DEPTH32FLOAT,
	DEPTH24STENCIL8
};

struct FramebufferAttachment {
	FramebufferPixelFormat format;
	glm::vec4 clearColor{1.0f};
};

struct FramebufferSpecifications {
	uint32_t width, height;
	bool isSwapChainTarget = false;
	bool isDepthEnabled = true;
	int samples = 1;

	std::vector<FramebufferAttachment> colorAttachments;
	FramebufferAttachment depthAttachment;
};

class Framebuffer {
  public:
	virtual ~Framebuffer() = default;

	virtual void* getPassDescriptor(void* drawable) = 0;

	virtual void* getDepthStencilState() const = 0;
	virtual void* setDepthStencilState() = 0;

	virtual void onResize(uint32_t width, uint32_t height) = 0;
	virtual void invalidate() = 0;

	virtual uint32_t getWidth() const = 0;
	virtual uint32_t getHeight() const = 0;

	virtual bool isSwapChainTarget() const = 0;
	virtual bool isDepthEnabled() const = 0;
	virtual FramebufferPixelFormat getDepthAttachmentPixelFormat() const = 0;
	virtual FramebufferPixelFormat getColorAttachmentPixelFormat(int index) const = 0;
	virtual int getRasterSampleCount() const = 0;

	static std::shared_ptr<Framebuffer> create(const FramebufferSpecifications& specs);

  protected:
	virtual void createColorTexture(int index) = 0;
	virtual void createMSAATexture(int index) = 0;
	virtual void createDepthTexture() = 0;
};

}  // namespace Atlas