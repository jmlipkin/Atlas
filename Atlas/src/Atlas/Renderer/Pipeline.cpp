#include "Pipeline.h"

#include "RendererAPI.h"
#include "atpch.h"

#include "Atlas/Platform/Metal/MetalPipeline.h"
#include "Atlas/Platform/OpenGL/OpenGLPipeline.h"

namespace Atlas {

std::shared_ptr<Pipeline> Pipeline::create(const PipelineSpecification& specs) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_ASSERT(false, "RendererAPI::None is not supported");
            break;
        }
        case RendererAPI::API::OpenGL: {
            return std::make_shared<OpenGLPipeline>(specs);
        }
        case RendererAPI::API::Metal: {
            return std::make_shared<MetalPipeline>(specs);
        }
    }

    AT_CORE_ERROR("RendererAPI not supported");
    return nullptr;
}

}  // namespace Atlas