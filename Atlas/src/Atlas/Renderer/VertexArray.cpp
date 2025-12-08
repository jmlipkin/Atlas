#include "atpch.h"
#include "VertexArray.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Platform/OpenGL/OpenGLVertexArray.h"

namespace Atlas {

    VertexArray* VertexArray::create() {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return new OpenGLVertexArray();
            }
            case RendererAPI::API::Metal: {
                AT_ASSERT(false, "RendererAPI::Metal is not supported");
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}