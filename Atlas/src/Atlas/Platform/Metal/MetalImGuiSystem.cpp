#include "GLFW/glfw3.h"
#include "atpch.h"
#include "MetalImGuiSystem.h"

#include "MetalContext.h"

#include <filesystem>

#include <imgui/imgui.h>

#include "Atlas/Core/Application.h"
#include "ImGuiMetalRenderer.h"


namespace Atlas {

void MetalImGuiSystem::initImGuiLayer() {
    AT_PROFILE_FUNCTION();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    io.IniFilename = "build/Atlas/extern/imgui.ini";
    if (std::filesystem::exists(io.IniFilename)) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
    }

	GLFWwindow* window = (GLFWwindow*)Application::get().getWindow().getNativeWindow();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	io.DisplaySize = ImVec2((float)width, (float)height);

	float font_scale;
	glfwGetWindowContentScale(window, &font_scale, nullptr);
	ImFont* HelveticaLight = io.Fonts->AddFontFromFileTTF("Atlas/assets/fonts/helvetica-neue-5/HelveticaNeueLight.otf", 14 * font_scale);
	ImFont* HelveticaBold = io.Fonts->AddFontFromFileTTF("Atlas/assets/fonts/helvetica-neue-5/HelveticaNeueBold.otf", 14 * font_scale);
	io.FontDefault = HelveticaLight;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    ImGui::StyleColorsDark();

    {
        AT_PROFILE_SCOPE("ImGui Metal init backend");
        
        ImGui_ImplMetal_Init(MetalContext::getMTLDevice());
    }
}

void MetalImGuiSystem::cleanImGuiLayer() {}

}  // namespace Atlas