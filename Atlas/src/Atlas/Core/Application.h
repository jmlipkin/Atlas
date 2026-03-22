#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Core/Window.h"
#include "Atlas/Core/LayerStack.h"
#include "Atlas/Core/RunMode.h"

#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"

#include "Atlas/Renderer/Framebuffer.h"
#include "Atlas/ImGui/ImGuiLayer.h"

#include <memory>

namespace Atlas {

class RuntimeLayer;

class Application {
  public:
	Application(const WindowProperties& winProps = WindowProperties());
	virtual ~Application();

	void run();
	void close() { m_isRunning = false; }

	void onEvent(Event& event);

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);

	inline Window&						getWindow() const { return *m_window; }
	inline std::shared_ptr<Framebuffer> getFramebuffer() { return m_sceneFrameBuf; }
	inline static Application&			get() { return *s_instance; }

	RunMode getRunMode() const { return m_runMode; }
	void	setRunMode(RunMode runMode);

  protected:
	DeltaTime m_dt;

  private:
	bool onWindowClose(WindowCloseEvent& e);
	bool onWindowResize(WindowResizeEvent& e);
	bool onWindowFocus(WindowFocusEvent& e);
	bool onWindowLostFocus(WindowLostFocusEvent& e);

  private:
	std::shared_ptr<Window>			 m_window;
	std::shared_ptr<GraphicsContext> m_context;
	std::shared_ptr<Framebuffer>	 m_sceneFrameBuf;
	std::shared_ptr<Framebuffer>	 m_swapchainBuf;
	bool							 m_isRunning   = true;
	bool							 m_isMinimized = false;

	LayerStack	  m_layerStack;
	ImGuiLayer*	  m_ImGuiLayer;
	RuntimeLayer* m_runtimeLayer;

	RunMode m_runMode = RunMode::BUILD;

	static Application* s_instance;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas