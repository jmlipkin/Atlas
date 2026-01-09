#include "Buffer.h"
#include "atpch.h"

#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"
#include "Atlas/Platform/Metal/MetalBuffer.h"
#include "Atlas/Renderer/Renderer.h"

#include "spdlog/fmt/bundled/core.h" // Required for fmt::ptr

namespace Atlas {

    std::shared_ptr<VertexBuffer> VertexBuffer::create(uint32_t size) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return std::make_shared<OpenGLVertexBuffer>(size);
            }
            case RendererAPI::API::Metal: {
                return std::make_shared<MetalVertexBuffer>(size);
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return std::make_shared<OpenGLVertexBuffer>(vertices, size);
            }
            case RendererAPI::API::Metal: {
                return std::make_shared<MetalVertexBuffer>(vertices, size);
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return std::make_shared<OpenGLIndexBuffer>(indices, size);
                break;
            }
            case RendererAPI::API::Metal: {
                return std::make_shared<MetalIndexBuffer>(indices, size);
                break;
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}  // namespace Atlas