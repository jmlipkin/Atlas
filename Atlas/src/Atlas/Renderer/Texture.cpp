#include "Texture.h"
#include "atpch.h"

#include "RendererAPI.h"
#include "Atlas/Platform/Metal/MetalTexture.h"

namespace Atlas {

std::shared_ptr<Texture> Texture::create(const std::string& filepath) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_CORE_ASSERT("RendererAPI::None is not supported");
            return nullptr;
        }
        case RendererAPI::API::OpenGL: {
            AT_CORE_ASSERT("RendererAPI::OpenGL is not supported");
            return nullptr;
        }
        case RendererAPI::API::Metal: {
            return std::make_shared<MetalTexture>(filepath);
        }
    }
}
std::shared_ptr<Texture> Texture::create(uint32_t width, uint32_t height) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_CORE_ASSERT("RendererAPI::None is not supported");
            return nullptr;
        }
        case RendererAPI::API::OpenGL: {
            AT_CORE_ASSERT("RendererAPI::OpenGL is not supported");
            return nullptr;
        }
        case RendererAPI::API::Metal: {
            return std::make_shared<MetalTexture>(width, height);
        }
    }
}
}  // namespace Atlas