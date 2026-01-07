#pragma once

#include <Atlas/ImGui/ImGuiLayer.h>

// TEMPORARY
#include <glm/glm.hpp>

namespace Atlas {

    class MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer();
        virtual ~MetalImGuiLayer() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate() override;

        // TEMPORARY
        static glm::vec4 getImGuiColor() { return triangleColor; }

       private:
        float m_time;

        // TEMPORARY
        static glm::vec4 triangleColor;
    };

}  // namespace Atlas