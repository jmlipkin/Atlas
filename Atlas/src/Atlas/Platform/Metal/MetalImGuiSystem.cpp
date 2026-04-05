#include "atpch.h"
#include "MetalImGuiSystem.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Platform.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include "Atlas/Platform/Metal/ImGuiMetalRenderer.h"
#include "Atlas/ImGui/EditorWidgets.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

namespace Atlas {

static GLFWwindow* getNativeWindow() {
	return (GLFWwindow*)Application::get().getWindow().getNativeWindow();
}

void MetalImGuiSystem::initImGuiLayer() {
	AT_PROFILE_FUNCTION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;

	static std::string iniPath = Platform::getResourcesPath() + "/imgui.ini";
	io.IniFilename			   = iniPath.c_str();
	if (std::filesystem::exists(io.IniFilename)) {
		ImGui::LoadIniSettingsFromDisk(io.IniFilename);
	}

	GLFWwindow* window = getNativeWindow();

	float scale;
	int	  width, height;

	glfwGetWindowSize(window, &width, &height);
	glfwGetWindowContentScale(window, &scale, nullptr);

	io.DisplaySize			   = ImVec2((float)width, (float)height);
	io.DisplayFramebufferScale = ImVec2(scale, scale);

	std::string fontpath = (Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueLight.otf");
	AT_CORE_ASSERT(std::filesystem::exists(fontpath), "Could not find font file: {}", fontpath);
	ImFont* HelveticaLight = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14.0 * scale);
	AT_CORE_ASSERT(HelveticaLight, "ImGui -- error loading font");

	fontpath = (Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueBold.otf");
	AT_CORE_ASSERT(std::filesystem::exists(fontpath), "Could not find font file: {}", fontpath);
	ImFont* HelveticaBold = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14.0 * scale);
	io.FontDefault		  = HelveticaLight;
	AT_CORE_ASSERT(HelveticaBold, "ImGui -- error loading font");

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	ImGui::StyleColorsDark();

	setupImGuiStyle(ImGui::GetStyle());
	ImGui::GetStyle().ScaleAllSizes(scale);
	ImGui::GetIO().FontGlobalScale = 1.0 / scale;
	EditorWidgets::displayScale	   = scale;

	{
		AT_PROFILE_SCOPE("ImGui Metal init backend");

		ImGui_ImplMetal_Init(MetalContext::getMTLDevice());
	}
}

void MetalImGuiSystem::cleanImGuiLayer() {}

void MetalImGuiSystem::rescale(float newScale) {
	ImGuiIO& io = ImGui::GetIO();

	GLFWwindow* window = getNativeWindow();
	int			width, height;
	glfwGetWindowSize(window, &width, &height);
	io.DisplaySize			   = ImVec2((float)width, (float)height);
	io.DisplayFramebufferScale = ImVec2(newScale, newScale);

	// Font rebuild
	io.Fonts->Clear();
	std::string fontpath = Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueLight.otf";
	ImFont*		light	 = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14.0f * newScale);
	io.FontDefault		 = light;
	fontpath			 = Platform::getResourcesPath() + "/Atlas/fonts/helvetica-neue-5/HelveticaNeueBold.otf";
	io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 14.0f * newScale);
	io.Fonts->Build();

	ImGui::GetStyle() = ImGuiStyle{};
	setupImGuiStyle(ImGui::GetStyle());
	ImGui::GetStyle().ScaleAllSizes(newScale);
	io.FontGlobalScale			= 1.0f / newScale;
	EditorWidgets::displayScale = newScale;
}

}  // namespace Atlas