#include "ImGuiSystem.h"
#include "atpch.h"

#include "Atlas/Platform/OpenGL/OpenGLImGuiSystem.h"
#include "Atlas/Platform/Metal/MetalImGuiSystem.h"

namespace Atlas {

std::unique_ptr<ImGuiSystem> ImGuiSystem::create() {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_ASSERT(false, "RendererAPI::None is not supported");
            break;
        }
        case RendererAPI::API::OpenGL: {
            return std::make_unique<OpenGLImGuiSystem>();
        }
        case RendererAPI::API::Metal: {
            return std::make_unique<MetalImGuiSystem>();
        }
    }
}

}  // namespace Atlas