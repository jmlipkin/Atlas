#include "atpch.h"
#include "GraphicsContext.h"

#include "RendererAPI.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include "Atlas/Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>

namespace Atlas {

    GraphicsContext* GraphicsContext::create(GLFWwindow* window) {
        switch(RendererAPI::getAPI()) {
            case RendererAPI::API::None: {
                AT_CORE_ASSERT(false, "RendererAPI::None is not currently supported.");
            }
            case RendererAPI::API::OpenGL: {
                AT_CORE_TRACE("Using RendererAPI::API::OpenGL");
                return new OpenGLContext(window);
            }
            case RendererAPI::API::Metal: {
                AT_CORE_TRACE("Using RendererAPI::API::Metal");
                return new MetalContext(window);
            }
        }
    }
}