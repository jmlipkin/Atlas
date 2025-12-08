#include "atpch.h"
#include "MetalImGuiLayer.h"

#include <imgui/imgui.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <metal-cpp/Metal.hpp>

#include <Atlas/Core.h>
#include <Atlas/Application.h>
#include <Atlas/KeyCodes.h>
#include <Atlas/MouseButtonCodes.h>
#include <Atlas/Platform/Metal/ImGuiMetalRenderer.h>
#include <Atlas/Platform/Metal/MetalContext.h>

namespace Atlas {

    MetalImGuiLayer::MetalImGuiLayer(const GraphicsContext& context) : ImGuiLayer() {
         m_mtlDevice = context.getMTLDevice();
    }

    void MetalImGuiLayer::onAttach() {
        ImGuiContext *ctx = ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.IniFilename = "build/Atlas/extern/imgui.ini";


        ImGui_ImplMetal_Init((id<MTLDevice>)m_mtlDevice);
    }

    void MetalImGuiLayer::onDetach() {

    }

    void MetalImGuiLayer::onUpdate() {
        // ImGuiIO& io = ImGui::GetIO();
		// Application& app = Application::get();
		// io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		// float time = (float)glfwGetTime();
		// io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		// m_time = time;

		// ImGui_ImplMetal3_NewFrame();
		// ImGui::NewFrame();

		// static bool show = true;
		// ImGui::ShowDemoWindow(&show);

		// ImGui::Render();
		// ImGui_ImplMetal3_RenderDrawData(ImGui::GetDrawData());
    }

}  // namespace Atlas