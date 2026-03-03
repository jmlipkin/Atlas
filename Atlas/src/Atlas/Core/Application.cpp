#include "Application.h"
#include "atpch.h"

#include "Atlas/Core/FontLibrary.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/ImGui/ImGuiLayer.h"
#include "Atlas/Renderer/RenderCommand.h"
#include "Atlas/Renderer/Renderer.h"

namespace Atlas {

Application* Application::s_instance = nullptr;

Application::Application(const WindowProperties& winProps) {
	AT_PROFILE_FUNCTION();

	AT_CORE_ASSERT(!s_instance, "Application already exists!");
	s_instance = this;

	m_window = std::shared_ptr<Window>(Window::create(winProps));
	m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));
	m_context = m_window->getGraphicsContext();
	Renderer::init(*m_context);
	FontLibrary::init();

	AT_CORE_INFO("Engine initialization complete!");

	m_ImGuiLayer = new ImGuiLayer;
	m_layerStack.pushOverlay(m_ImGuiLayer);

	m_sceneFrameBuf = Framebuffer::create({m_context->getWidth(), m_context->getHeight(), false, true, 4, {{FramebufferPixelFormat::RGBA8, glm::vec4(0.00f, 0.0f, 0.0f, 1.0f)}}, {FramebufferPixelFormat::DEPTH32FLOAT, glm::vec4(0.0f)}});
	m_swapchainBuf = Framebuffer::create({m_context->getWidth(), m_context->getHeight(), true, true, 4, {{FramebufferPixelFormat::RGBA8, glm::vec4(0.0f)}}, {FramebufferPixelFormat::DEPTH32FLOAT, glm::vec4(0.0f)}});
}

Application::~Application() {
	Renderer::shutdown();
}

void Application::run() {
	AT_PROFILE_FUNCTION();

	while (m_isRunning) {
		m_dt.updateDelta();
		m_window->onUpdate();

		RenderCommand::beginFrame();
		RenderCommand::beginPass(m_sceneFrameBuf);

		if (!m_isMinimized) {
			for (Layer* l : m_layerStack) {
				l->onUpdate(m_dt);
			}
		}

		RenderCommand::endPass();

		RenderCommand::beginPass(m_swapchainBuf);
		m_ImGuiLayer->begin();
		if (!m_isMinimized) {
			for (Layer* l : m_layerStack) {
				l->onImGuiRender();
			}
		}
		m_ImGuiLayer->end();

		RenderCommand::endPass();
		RenderCommand::endFrame();
	}
}

void Application::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<WindowCloseEvent>(AT_BIND_EVENT_FN(Application::onWindowClose));
	dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(Application::onWindowResize));
	dispatcher.dispatch<WindowFocusEvent>(AT_BIND_EVENT_FN(Application::onWindowFocus));
	dispatcher.dispatch<WindowLostFocusEvent>(AT_BIND_EVENT_FN(Application::onWindowLostFocus));

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
	m_context->onResize(e);
	m_sceneFrameBuf->onResize(e.getWidth(), e.getHeight());
	m_swapchainBuf->onResize(e.getWidth(), e.getHeight());

	if (e.getHeight() == 0 || e.getWidth() == 0) {
		m_isMinimized = true;
		return false;
	}

	m_isMinimized = false;
	return false;
}

bool Application::onWindowFocus(WindowFocusEvent& e) {
	return false;
}

bool Application::onWindowLostFocus(WindowLostFocusEvent& e) {
	return false;
}

}  // namespace Atlas