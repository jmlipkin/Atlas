#include "MetalTexture.h"

#include <stb_image/stb_image.h>
#include <cstdint>

#include "Atlas/Renderer/Texture.h"
#include "MetalContext.h"
#include "atpch.h"
#include "metal-cpp/Metal.hpp"

namespace Atlas {

MetalTexture::MetalTexture(const std::string& filepath) : m_path(filepath) {
    AT_PROFILE_FUNCTION();

	MTL::Device* device;
	{
		AT_PROFILE_SCOPE("Get Metal Device");

		device = MetalContext::getMTLDevice();
	}

	unsigned char* image;
	{
		AT_PROFILE_SCOPE("stbi_load");

		int width, height, channels;
		image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		AT_CORE_ASSERT(image, "Image at [{}] could not be loaded.", filepath);

		m_width = (uint32_t)width;
		m_height = (uint32_t)height;
		m_channels = (uint32_t)channels;
	}

	MTL::TextureDescriptor* textureDescriptor;
	{
		AT_PROFILE_SCOPE("Create new MTL texture");

		textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(m_width);
		textureDescriptor->setHeight(m_height);

		m_texture = device->newTexture(textureDescriptor);
	}

	{
		AT_PROFILE_SCOPE("Bind image to GPU");

		MTL::Region region = MTL::Region(0, 0, 0, m_width, m_height, 1);
		NS::UInteger bytesPerRow = 4 * m_width;

		m_texture->replaceRegion(region, 0, image, bytesPerRow);
	}

	textureDescriptor->release();
	stbi_image_free(image);
}

MetalTexture::MetalTexture(TextureSpecification specs, void* data) : m_width(specs.width), m_height(specs.height) {
	MTL::Device* device = MetalContext::getMTLDevice();
	MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();

	textureDescriptor->setPixelFormat(toMTLPixelFormat(specs.format));
	textureDescriptor->setWidth(m_width);
	textureDescriptor->setHeight(m_height);
    
	m_texture = device->newTexture(textureDescriptor);

    if(data) {
        MTL::Region region = MTL::Region(0, 0, 0, m_width, m_height, 1);
        NS::UInteger bytesPerRow = Texture::formatSize(specs.format) * m_width;

        m_texture->replaceRegion(region, 0, data, bytesPerRow);
    }

	textureDescriptor->release();
}

MetalTexture::MetalTexture(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();

    MTL::Device* device = MetalContext::getMTLDevice();
    m_texture = device->newTexture(textureDescriptor);
    textureDescriptor->release();
}

void MetalTexture::setData(void* data, uint32_t size) {
	AT_PROFILE_FUNCTION();

	MTL::Device* device = MetalContext::getMTLDevice();

	MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
	textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
	textureDescriptor->setWidth(m_width);
	textureDescriptor->setHeight(m_height);

	m_texture = device->newTexture(textureDescriptor);

	MTL::Region region = MTL::Region(0, 0, 0, m_width, m_height, 1);
	NS::UInteger bytesPerRow = 4 * m_width;

	m_texture->replaceRegion(region, 0, data, bytesPerRow);

	textureDescriptor->release();
}

MetalTexture::~MetalTexture() {
	m_texture->release();
}

MTL::PixelFormat MetalTexture::toMTLPixelFormat(ImageFormat format) {
	switch (format) {
		case ImageFormat::R8:
			return MTL::PixelFormatR8Unorm;
		case ImageFormat::RGBA8:
			return MTL::PixelFormatRGBA8Uint;
		case ImageFormat::RGBA32F:
			return MTL::PixelFormatRGBA32Float;
		default:
			AT_CORE_ASSERT(false, "Unsupported texture format!");
	}
}

}  // namespace Atlas