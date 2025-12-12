#pragma once

#include "RendererAPI.h"

namespace Atlas {

    class RenderCommand {
        public:
         inline static void setClearColor(const glm::vec4& color) { s_rendererAPI->setClearColor(color); }
         inline static void clear() { s_rendererAPI->clear(); }
         inline static void commit() { s_rendererAPI->commit(); }

         inline static void onEvent(Event& event) { s_rendererAPI->onEvent(event); }

         inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_rendererAPI->drawIndexed(vertexArray); }

         static void init();

        private:
         static RendererAPI* s_rendererAPI;
    };

}  // namespace Atlas