#include "MetalTexture.h"
#include "atpch.h"

#include "MetalContext.h"

#include <stb_image/stb_image.h>


namespace Atlas {

MetalTexture::MetalTexture(const std::string& filepath) {
    MTL::Device* device = MetalContext::getMTLDevice();

    int width, height, channels;
    unsigned char* image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    AT_CORE_ASSERT(image, "Image at [{}] could not be loaded.", filepath);

    m_width = (uint32_t)width;
    m_height = (uint32_t)height;
    m_channels = (uint32_t)channels;

    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    textureDescriptor->setWidth(m_width);
    textureDescriptor->setHeight(m_height);

    m_texture = device->newTexture(textureDescriptor);

    MTL::Region region = MTL::Region(0, 0, 0, m_width, m_height, 1);
    NS::UInteger bytesPerRow = 4 * m_width;

    m_texture->replaceRegion(region, 0, image, bytesPerRow);

    textureDescriptor->release();
    stbi_image_free(image);
}

MetalTexture::MetalTexture(uint32_t width, uint32_t height) : m_width(width), m_height(height), m_texture(nullptr) {}

void MetalTexture::setData(void* data, uint32_t size) {
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

}  // namespace Atlas