#pragma once

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>
#include <Atlas/Core/Layer.h>

#include <Atlas/Renderer/GraphicsContext.h>
#include "Atlas/ImGui/ImGuiSystem.h"
#include "Atlas/Renderer/Renderer.h"

namespace Atlas {

class ImGuiLayer : public Layer {
  public:
	ImGuiLayer();
	virtual ~ImGuiLayer() = default;

	void onUpdate(DeltaTime dt) override {}

	void onAttach() final { m_system->initImGuiLayer(); }
	void onDetach() final { m_system->cleanImGuiLayer(); }

	virtual void onEvent(Event& event) override;

	virtual void begin() { Renderer::beginImGui(); }
	virtual void end() { Renderer::submitImGui(); }

  protected:
	bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	bool onMouseMovedEvent(MouseMovedEvent& e);
	bool onMouseScrolledEvent(MouseScrolledEvent& e);
	bool onKeyPressedEvent(KeyPressedEvent& e);
	bool onKeyReleasedEvent(KeyReleasedEvent& e);
	bool onKeyTypedEvent(KeyTypedEvent& e);
	bool onWindowResizeEvent(WindowResizeEvent& e);
	bool onDisplayChangedEvent(DisplayChangedEvent& e);

  protected:
	static GraphicsContext* m_context;
	float					m_time = 0.0f;

  private:
	std::unique_ptr<ImGuiSystem> m_system;
};

}  // namespace Atlas