#include "atpch.h"
#include "VertexArray.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Platform/OpenGL/OpenGLVertexArray.h"
#include "Atlas/Platform/Metal/MetalVertexArray.h"

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
                return new MetalVertexArray();
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}