#pragma once
#include "Atlas/Renderer/RendererAPI.h"

#include "MetalContext.h"

namespace Atlas {

    class MetalRendererAPI : public RendererAPI {
    public:
     MetalRendererAPI();
     virtual ~MetalRendererAPI() = default;
     virtual void setClearColor(const glm::vec4& color) override;
     virtual void clear() override;
     virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

     private:
      const MTL::Device& pDevice;
    };

}  // namespace Atlas