#pragma once

#include <Atlas/ImGui/ImGuiLayer.h>
#include "Atlas/Platform/Metal/MetalContext.h"

namespace Atlas {

    class MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer();
        virtual ~MetalImGuiLayer() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate() override;

        private:
         MetalContextData* m_MTLData;
    };

}  // namespace Atlas