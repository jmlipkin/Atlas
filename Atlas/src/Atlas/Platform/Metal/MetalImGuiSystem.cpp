#include "GLFW/glfw3.h"
#include "atpch.h"
#include "MetalImGuiSystem.h"

#include "MetalContext.h"

#include <filesystem>

#include <imgui/imgui.h>

#include "Atlas/AtlasPaths.h"
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

    std::string fontpath = (std::string(FONT_LIBRARY_PATH) + "helvetica-neue-5/HelveticaNeueLight.otf");
	ImFont* HelveticaLight = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14 * font_scale);
    AT_CORE_ASSERT(HelveticaLight, "ImGui -- error loading font");

    fontpath = (std::string(FONT_LIBRARY_PATH) + "helvetica-neue-5/HelveticaNeueBold.otf");
	ImFont* HelveticaBold = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14 * font_scale);
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