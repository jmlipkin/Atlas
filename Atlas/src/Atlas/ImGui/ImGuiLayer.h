#pragma once

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>
#include <Atlas/Layer.h>

namespace Atlas {

class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    virtual ~ImGuiLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;

    virtual void onEvent(Event& event) override;

   private:
    bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
    bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
    bool onMouseMovedEvent(MouseMovedEvent& e);
    bool onMouseScrolledEvent(MouseScrolledEvent& e);
    bool onKeyPressedEvent(KeyPressedEvent& e);
    bool onKeyReleasedEvent(KeyReleasedEvent& e);
    bool onKeyTypedEvent(KeyTypedEvent& e);
    bool onWindowResizeEvent(WindowResizeEvent& e);

   private:
    float m_time = 0.0f;
};

}  // namespace Atlas