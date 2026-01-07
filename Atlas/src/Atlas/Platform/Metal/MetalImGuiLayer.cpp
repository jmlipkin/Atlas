#include "atpch.h"
#include "MetalImGuiLayer.h"

#include "MetalContext.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/KeyCodes.h"
#include "Atlas/Core/MouseButtonCodes.h"

#include "Atlas/Renderer/Renderer.h"

#include <filesystem>

#include <imgui/imgui.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "ImGuiMetalRenderer.h"


namespace Atlas {

// TEMP
glm::vec4 MetalImGuiLayer::triangleColor = glm::vec4(1.0f);

MetalImGuiLayer::MetalImGuiLayer() : ImGuiLayer() {}

void MetalImGuiLayer::onAttach() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = "build/Atlas/extern/imgui.ini";
    if (std::filesystem::exists(io.IniFilename)) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
    }

    Application& app = Application::get();
    io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    ImGui::StyleColorsDark();

    ImGui_ImplMetal_Init(MetalContext::getMTLDevice());
}

    void MetalImGuiLayer::onDetach() {}

    void MetalImGuiLayer::onUpdate() {
		GLFWwindow* window = (GLFWwindow*)Application::get().getWindow().getNativeWindow();

        int winWidth, winHeight;
        int fbWidth, fbHeight;

        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)winWidth, (float)winHeight);
        io.DisplayFramebufferScale = ImVec2(
            (float)fbWidth / (float)winWidth,
            (float)fbHeight / (float)winHeight
        );

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

        Renderer::beginImGui();
        ImGui::NewFrame();
 
        // --- EXAMPLE UI ---
        static bool showDemo = true;
        ImGui::ShowDemoWindow(&showDemo);

        ImGui::Begin("Triangle Color");
        ImGui::ColorEdit4("Color", &triangleColor[0]);
        ImGui::End();

        ImGui::Render();
        Renderer::submitImGui();
    }

}  // namespace Atlas