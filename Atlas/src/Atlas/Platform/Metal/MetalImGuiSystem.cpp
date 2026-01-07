#include "atpch.h"
#include "MetalImGuiSystem.h"

#include "MetalContext.h"

#include <filesystem>

#include <imgui/imgui.h>
// #define GLFW_INCLUDE_NONE
// #include <GLFW/glfw3.h>
// #define GLFW_EXPOSE_NATIVE_COCOA
// #include <GLFW/glfw3native.h>

#include "Atlas/Core/Application.h"
#include "ImGuiMetalRenderer.h"


namespace Atlas {

void MetalImGuiSystem::initImGuiLayer() {
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

void MetalImGuiSystem::cleanImGuiLayer() {}

}  // namespace Atlas