#pragma once

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>
#include <Atlas/Core/Layer.h>

#include <Atlas/Renderer/GraphicsContext.h>
#include "Atlas/ImGui/ImGuiSystem.h"

namespace Atlas {

class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    virtual ~ImGuiLayer() = default;

        virtual void onImGuiRender() = 0;

        void onUpdate() final;

        void onAttach() final { m_system->initImGuiLayer(); }
        void onDetach() final { m_system->cleanImGuiLayer(); }

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

        private:
         std::unique_ptr<ImGuiSystem> m_system;
};

}  // namespace Atlas