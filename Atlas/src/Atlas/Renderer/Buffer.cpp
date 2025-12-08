#include "Buffer.h"
#include "atpch.h"

#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"
#include "Atlas/Renderer/Renderer.h"

#include "spdlog/fmt/bundled/core.h" // Required for fmt::ptr

namespace Atlas {

    VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return new OpenGLVertexBuffer(vertices, size);
            }
            case RendererAPI::API::Metal: {
                AT_ASSERT(false, "RendererAPI::Metal is not supported");
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t size) {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return new OpenGLIndexBuffer(indices, size);
            }
            case RendererAPI::API::Metal: {
                AT_ASSERT(false, "RendererAPI::Metal is not supported");
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}  // namespace Atlas