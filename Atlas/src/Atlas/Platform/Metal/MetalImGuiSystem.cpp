#include "atpch.h"
#include "MetalImGuiSystem.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Platform.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include "Atlas/Platform/Metal/ImGuiMetalRenderer.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

namespace Atlas {

void MetalImGuiSystem::initImGuiLayer() {
    AT_PROFILE_FUNCTION();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    static std::string iniPath = Platform::getResourcesPath() + "/imgui.ini";
    io.IniFilename = iniPath.c_str();
    if (std::filesystem::exists(io.IniFilename)) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
    }

	GLFWwindow* window = (GLFWwindow*)Application::get().getWindow().getNativeWindow();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	io.DisplaySize = ImVec2((float)width, (float)height);

	float font_scale;
	glfwGetWindowContentScale(window, &font_scale, nullptr);

    std::string fontpath = (Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueLight.otf");
    AT_CORE_ASSERT(std::filesystem::exists(fontpath), "Could not find font file: {}", fontpath);
	ImFont* HelveticaLight = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14 * font_scale);
    AT_CORE_ASSERT(HelveticaLight, "ImGui -- error loading font");
    
    fontpath = (Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueBold.otf");
    AT_CORE_ASSERT(std::filesystem::exists(fontpath), "Could not find font file: {}", fontpath);
	ImFont* HelveticaBold = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14 * font_scale);
	io.FontDefault = HelveticaLight;
    AT_CORE_ASSERT(HelveticaBold, "ImGui -- error loading font");

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