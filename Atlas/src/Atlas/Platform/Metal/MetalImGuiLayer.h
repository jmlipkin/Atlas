#pragma once

#include <Atlas/ImGui/ImGuiLayer.h>
#include <Atlas/Renderer/GraphicsContext.h>

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    class MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer(const GraphicsContext& context);
        virtual ~MetalImGuiLayer() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate() override;

        private:
         MTL::Device* m_mtlDevice;
    };

}  // namespace Atlas