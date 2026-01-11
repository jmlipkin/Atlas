#pragma once

#include <metal-cpp/Metal.hpp>

#include "Atlas/Renderer/Texture.h"

namespace Atlas {

class MetalTexture : public Texture {
   public:
    MetalTexture(const std::string& filepath);
    MetalTexture(uint32_t width, uint32_t height);
    virtual ~MetalTexture() override;

    MTL::Texture* getMTLTexture() const { return m_texture; }

    virtual void setData(void* data, uint32_t size) override;

    virtual bool operator==(const Texture& other) const override { return m_texture == ((MetalTexture&)other).m_texture; }

    virtual uint32_t getWidth() const override { return m_width; }
    virtual uint32_t getHeight() const override { return m_height; }

   private:
    MTL::Texture* m_texture;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_channels;
};

}  // namespace Atlas