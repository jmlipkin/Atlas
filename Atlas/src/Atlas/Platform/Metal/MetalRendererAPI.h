#pragma once

#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Window.h"
#include "MetalContext.h"

namespace Atlas {

class MetalRendererAPI : public RendererAPI {
   public:
    MetalRendererAPI(std::shared_ptr<Window> window);
    virtual ~MetalRendererAPI();

    virtual void setClearColor(const glm::vec4& color) override;
    virtual void clear() override;

    virtual void beginScene() override;

    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    virtual void commit() override;

    virtual void onEvent(Event& event) override;

   private:
    bool onWindowResize(WindowResizeEvent& e);

   private:
    MTL::ClearColor m_color;
};

}  // namespace Atlas