#pragma once
#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas {

    class MetalRendererAPI : public RendererAPI {
    public:
        virtual void setClearColor(const glm::vec4& color) override;
        virtual void clear() override;

    };

}  // namespace Atlas