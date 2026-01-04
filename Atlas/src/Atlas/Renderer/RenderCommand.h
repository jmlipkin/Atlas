#pragma once

#include "RendererAPI.h"
#include "Atlas/Window.h"

namespace Atlas {

    class RenderCommand {
        public:
         inline static void setClearColor(const glm::vec4& color) { s_rendererAPI->setClearColor(color); }
         inline static void clear() { s_rendererAPI->clear(); }
         inline static void commit() { s_rendererAPI->commit(); }

         inline static void beginScene() { s_rendererAPI->beginScene(); }

         inline static void onEvent(Event& event) { s_rendererAPI->onEvent(event); }

         inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawIndexed(vertexArray); }

         static void init(std::shared_ptr<Window> window);

        private:
         static RendererAPI* s_rendererAPI;
    };

}  // namespace Atlas