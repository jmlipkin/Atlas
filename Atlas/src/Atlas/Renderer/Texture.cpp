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
std::shared_ptr<Texture> Texture::create(TextureSpecification specs, void* data) {
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
            return std::make_shared<MetalTexture>(specs, data);
        }
    }
}

uint32_t Texture::formatSize(ImageFormat format) {
	switch (format) {
		case ImageFormat::R8:
			return 1;
        case ImageFormat::RGB8:
            return 3;
		case ImageFormat::RGBA8:
			return 4;
		case ImageFormat::RGBA32F:
			return 16;
		default:
			AT_CORE_ASSERT(false, "Unsupported texture format!");
	}
}
}  // namespace Atlas