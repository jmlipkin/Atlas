#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Atlas {

class Application {
   public:
    Application();
    virtual ~Application();

    void run();

    void onEvent(Event& event);

    private:
     bool onWindowClose(WindowCloseEvent& e);

    private:
     std::unique_ptr<Window> m_window;
     bool m_isRunning = true;

     LayerStack m_layerStack;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas