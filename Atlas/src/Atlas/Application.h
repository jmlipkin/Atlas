#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Atlas {

class Application {
   public:
    Application();
    virtual ~Application();

    void run();
    private:
     std::unique_ptr<Window> m_window;
     bool m_isRunning = true;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas