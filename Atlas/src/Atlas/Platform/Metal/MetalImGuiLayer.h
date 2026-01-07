#pragma once

#include <Atlas/ImGui/ImGuiLayer.h>

namespace Atlas {

    class MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer();
        virtual ~MetalImGuiLayer() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate() override;
    private:
     float m_time;
    };

}  // namespace Atlas