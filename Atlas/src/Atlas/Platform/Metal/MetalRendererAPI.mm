#include "atpch.h"
#include "MetalRendererAPI.h"

#include <imgui/imgui.h>
#include "ImGuiMetalRenderer.h"

namespace Atlas {
    void MetalRendererAPI::beginImGui() {
        ImGui_ImplMetal_NewFrame((MTLRenderPassDescriptor*)m_passDesc);
    }

    void MetalRendererAPI::drawImGui() {
        ImGui_ImplMetal_RenderDrawData(
            ImGui::GetDrawData(),
            id<MTLCommandBuffer>(m_buffer),
            id<MTLRenderCommandEncoder>(m_encoder)
        );
    }
}