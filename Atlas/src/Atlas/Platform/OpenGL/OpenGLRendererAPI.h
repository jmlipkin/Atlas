#pragma once

#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas {

    class OpenGLRendererAPI : public RendererAPI {
       public:
        virtual ~OpenGLRendererAPI() = default;
        virtual void setClearColor(const glm::vec4& color) override;
        virtual void clear() override;
        virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    };

}  // namespace Atlas