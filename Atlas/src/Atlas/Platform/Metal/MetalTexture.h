#pragma once

#include <metal-cpp/Metal.hpp>

#include "Atlas/Renderer/Texture.h"

namespace Atlas {

class MetalTexture : public Texture {
  public:
	MetalTexture(const std::string& filepath);
	MetalTexture(uint32_t width, uint32_t height);
	MetalTexture(TextureSpecification specs, void* data = nullptr);
	virtual ~MetalTexture() override;

	virtual void* getData() const override { return m_texture; }
	MTL::Texture* getMTLTexture() const { return m_texture; }

	virtual std::string& getFilepath() override { return m_path; }
	virtual std::string	 getFilepath() const override { return m_path; }

	virtual void setData(void* data, uint32_t size) override;

	virtual bool operator==(const Texture& other) const override { return m_texture == ((MetalTexture&)other).m_texture; }

	virtual uint32_t getWidth() const override { return m_width; }
	virtual uint32_t getHeight() const override { return m_height; }

	static MTL::PixelFormat toMTLPixelFormat(ImageFormat format);

  private:
	MTL::Texture* m_texture;

	std::string m_path;
	uint32_t	m_width;
	uint32_t	m_height;
	uint32_t	m_channels;
};

}  // namespace Atlas