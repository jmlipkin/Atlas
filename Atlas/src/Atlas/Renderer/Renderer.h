#pragma once

#include "RenderCommand.h"
namespace Atlas {

    class Renderer {
        public:
         static void init(std::shared_ptr<Window> window);

         static void beginScene();
         static void endScene();

         static void submit(const std::shared_ptr<VertexArray>& vertexArray);

         inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
    };

}  // namespace Atlas