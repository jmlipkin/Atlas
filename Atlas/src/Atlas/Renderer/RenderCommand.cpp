#include "atpch.h"

#include "RenderCommand.h"

#include "Atlas/Platform/Metal/MetalRendererAPI.h"
#include "Atlas/Platform/OpenGL/OpenGLRendererAPI.h"

#include "Atlas/Platform/Metal/MetalBuffer.h"
#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"

namespace Atlas {

RendererAPI* RenderCommand::s_rendererAPI = nullptr;

void RenderCommand::init(GraphicsContext& context) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            s_rendererAPI = nullptr;
            AT_ASSERT(false, "RendererAPI::None is not supported");
            break;
        }
        case RendererAPI::API::OpenGL: {
            s_rendererAPI = new OpenGLRendererAPI;
            break;
        }
        case RendererAPI::API::Metal: {
            MetalContext& metalContext = static_cast<MetalContext&>(context);
            s_rendererAPI = new MetalRendererAPI(metalContext);
            break;
        }
    }
}
}  // namespace Atlas