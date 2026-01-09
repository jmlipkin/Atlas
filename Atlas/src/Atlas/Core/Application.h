#pragma once

#include "Base.h"

#include "Window.h"
#include "LayerStack.h"

#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"

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
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas