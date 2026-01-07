#pragma once

#include "RenderCommand.h"
#include "Pipeline.h"
namespace Atlas {

    class Renderer {
        public:
         static void init(GraphicsContext& context);

         static void beginScene();
         static void endScene();

         static void beginImGui();
         static void submitImGui();

         static void submit(const Pipeline& pipeline, const std::shared_ptr<VertexArray>& vertexArray);

         inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
    };

}  // namespace Atlas