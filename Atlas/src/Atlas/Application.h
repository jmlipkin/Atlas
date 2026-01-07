#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Pipeline.h"

namespace Atlas {

class Application {
   public:
    Application();
    virtual ~Application() = default;

    void run();

    void onEvent(Event& event);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    inline Window& getWindow() const { return *m_window; }
    inline static Application& get() { return *s_instance; }

   private:
    bool onWindowClose(WindowCloseEvent& e);

   private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<GraphicsContext> m_context;
    bool m_isRunning = true;

    LayerStack m_layerStack;

    static Application* s_instance;

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray;

    std::shared_ptr<Pipeline> m_pipeline;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas