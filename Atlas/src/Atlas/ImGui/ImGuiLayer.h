#pragma once

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>
#include <Atlas/Layer.h>

#include <Atlas/Renderer/GraphicsContext.h>

namespace Atlas {

class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    virtual ~ImGuiLayer() = default;

    static void setImGuiGraphicsContextPtr(const void* context) { m_context = static_cast<GraphicsContext*>(const_cast<void*>(context)); }

        static ImGuiLayer* create();

        virtual void onAttach() override = 0;
        virtual void onDetach() override = 0;
        virtual void onUpdate() override = 0;

        virtual void onEvent(Event & event) override;

       protected:
        bool onMouseButtonPressedEvent(MouseButtonPressedEvent & e);
        bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent & e);
        bool onMouseMovedEvent(MouseMovedEvent & e);
        bool onMouseScrolledEvent(MouseScrolledEvent & e);
        bool onKeyPressedEvent(KeyPressedEvent & e);
        bool onKeyReleasedEvent(KeyReleasedEvent & e);
        bool onKeyTypedEvent(KeyTypedEvent & e);
        bool onWindowResizeEvent(WindowResizeEvent & e);

       protected:
        static GraphicsContext* m_context;
        float m_time = 0.0f;
};

}  // namespace Atlas