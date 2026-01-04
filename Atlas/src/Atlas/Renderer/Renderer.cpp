#include "atpch.h"
#include "Renderer.h"

namespace Atlas {

        void Renderer::init(std::shared_ptr<Window> window) {
            RenderCommand::init(window);
            AT_CORE_TRACE("Initialized Renderer");
        }

        void Renderer::beginScene() {
            RenderCommand::beginScene();
        }

        void Renderer::endScene() {
        }

        void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray) {
            vertexArray->bind();
            RenderCommand::drawIndexed(vertexArray);
        }
}