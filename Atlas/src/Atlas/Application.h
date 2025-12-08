#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

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
    std::unique_ptr<Window> m_window;
    bool m_isRunning = true;

    LayerStack m_layerStack;

    static Application* s_instance;

    std::shared_ptr<Shader> m_shader, m_blueShader;
    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray, m_squareVA;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas