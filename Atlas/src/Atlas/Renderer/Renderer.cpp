#include "atpch.h"
#include "Renderer.h"

namespace Atlas {

        void Renderer::init() {
            RenderCommand::init();
            AT_CORE_TRACE("Initialized Renderer");
        }

        void Renderer::beginScene() {

        }

        void Renderer::endScene() {

        }

        void Renderer::submit(const std::shared_ptr<VertexArray>& vertexArray) {
            vertexArray->bind();
            RenderCommand::drawIndexed(vertexArray);
        }
}