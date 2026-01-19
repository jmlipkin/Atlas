#pragma once

#include "Window.h"
#include "LayerStack.h"

#include "Atlas/Core/Time.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Renderer/Framebuffer.h"

namespace Atlas {

class Application {
   public:
    Application(const WindowProperties& winProps = WindowProperties());
    virtual ~Application();

    void run();

    void onEvent(Event& event);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    inline Window& getWindow() const { return *m_window; }
    inline static Application& get() { return *s_instance; }

   protected:
    DeltaTime m_dt;

   private:
    bool onWindowClose(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);
    bool onWindowFocus(WindowFocusEvent& e);
    bool onWindowLostFocus(WindowLostFocusEvent& e);

    private:
    std::shared_ptr<Window> m_window;
	std::shared_ptr<GraphicsContext> m_context;
	std::shared_ptr<Framebuffer> m_framebuf;
    bool m_isRunning = true;
    bool m_isMinimized = false;

	LayerStack m_layerStack;

    static Application* s_instance;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas