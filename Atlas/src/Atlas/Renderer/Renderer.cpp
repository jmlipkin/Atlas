#include "atpch.h"
#include "Renderer.h"

namespace Atlas {

        void Renderer::init(GraphicsContext& context) {
            RenderCommand::init(context);
            AT_CORE_TRACE("Renderer initialized");
        }

        void Renderer::beginScene() {
        }

        void Renderer::endScene() {
        }

        void Renderer::beginImGui() {
            RenderCommand::beginImGui();
        }

        void Renderer::submitImGui() {
            RenderCommand::drawImGui();
        }

        void Renderer::submit(const Pipeline& pipeline, const std::shared_ptr<VertexArray>& vertexArray, const UniformBuffer& uBuffer) {
            RenderCommand::bindPipeline(pipeline, uBuffer);
            RenderCommand::bindVertexArray(*vertexArray);
            RenderCommand::drawIndexed(vertexArray);
        }
}