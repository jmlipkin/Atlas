#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Atlas {

    Application::Application() {
        m_window = std::unique_ptr<Window>(Window::create());
    }

    Application::~Application() {}

    void Application::run() {

        while (m_isRunning) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_window->onUpdate();
        }
    }
}