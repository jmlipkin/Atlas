#pragma once

#include <Atlas/ImGui/ImGuiLayer.h>

namespace Atlas {

    class OpenGLImGuiLayer : public ImGuiLayer {
    public:
        OpenGLImGuiLayer();
        virtual ~OpenGLImGuiLayer();

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate() override;
    };

}  // namespace Atlas