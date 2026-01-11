#include "Application.h"

#include "Atlas/Core/Input.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/ImGui/ImGuiLayer.h"
#include "Atlas/Renderer/Renderer.h"
#include "atpch.h"

namespace Atlas {

Application* Application::s_instance = nullptr;

Application::Application() {
    AT_PROFILE_FUNCTION();

    AT_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = std::shared_ptr<Window>(Window::create());
    m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));
    m_context = m_window->getGraphicsContext();
    Renderer::init(*m_context);

    AT_CORE_INFO("Engine initialization complete!");
}

Application::~Application() {
    Renderer::shutdown();
}

void Application::run() {
    AT_PROFILE_FUNCTION();
    RenderCommand::setClearColor({0.15f, 0.15f, 0.15f, 1.0f});

    while (m_isRunning) {
        m_dt.updateDelta();
        m_window->onUpdate();

        RenderCommand::beginFrame();

        if(!m_isMinimized) {
            for (Layer* l : m_layerStack) {
                l->onUpdate(m_dt);
            }
        }

        RenderCommand::endFrame();
    }
}

void Application::onEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<WindowCloseEvent>(AT_BIND_EVENT_FN(Application::onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(Application::onWindowResize));

    for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
        (*--it)->onEvent(event);
        if (event.isHandled)
            break;
    }
}

void Application::pushLayer(Layer* layer) {
    m_layerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay) {
    m_layerStack.pushOverlay(overlay);
}

bool Application::onWindowClose(WindowCloseEvent& e) {
    m_isRunning = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {
    if(e.getHeight() == 0 || e.getWidth() == 0) {
        m_isMinimized = true;
        return false;
    }

    m_isMinimized = false;
    return false;
}
}  // namespace Atlas