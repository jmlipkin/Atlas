#include "atpch.h"
#include "RenderCommand.h"

#include "Atlas/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Atlas/Platform/Metal/MetalRendererAPI.h"

namespace Atlas {

RendererAPI* RenderCommand::s_rendererAPI = nullptr;

void RenderCommand::init() {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            s_rendererAPI = nullptr;
            AT_ASSERT(false, "RendererAPI::None is not supported");
        }
        case RendererAPI::API::OpenGL: {
            s_rendererAPI = new OpenGLRendererAPI;
        }
        case RendererAPI::API::Metal: {
            s_rendererAPI = new MetalRendererAPI;
        }
    }
    }
}