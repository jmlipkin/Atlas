#include "atpch.h"
#include "RenderCommand.h"

#include "Atlas/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Atlas {
    RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;
}