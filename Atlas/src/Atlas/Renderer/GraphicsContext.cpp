#include "atpch.h"
#include "GraphicsContext.h"

#include "Atlas/Renderer/Renderer.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include "Atlas/Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>

namespace Atlas {

    std::shared_ptr<GraphicsContext> GraphicsContext::create(GLFWwindow* window) {
        switch(Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_CORE_ASSERT(false, "RendererAPI::None is not currently supported.");
            }
            case RendererAPI::API::OpenGL: {
                AT_CORE_TRACE("Using OpenGL for Rendering API");
                return std::make_shared<OpenGLContext>(window);
            }
            case RendererAPI::API::Metal: {
                AT_CORE_INFO("Using Metal for Renderering API");
                return std::make_shared<MetalContext>(window);
            }
        }
    }
}