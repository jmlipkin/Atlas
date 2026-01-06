#include "atpch.h"
#include "Renderer.h"

namespace Atlas {

        void Renderer::init(GraphicsContext& context) {
            RenderCommand::init(context);
            AT_CORE_TRACE("Initialized Renderer");
        }

        void Renderer::beginScene() {
        }

        void Renderer::endScene() {
        }

        void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray) {
            RenderCommand::bindVertexArray(*vertexArray);
            RenderCommand::drawIndexed(vertexArray);
        }
}