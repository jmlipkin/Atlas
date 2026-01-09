#include "OpenGLImGuiSystem.h"

#include <glad/glad.h>
#include <imgui/imgui.h>

#include "atpch.h"

#include "ImGuiOpenGLRenderer.h"

namespace Atlas {

void OpenGLImGuiSystem::initImGuiLayer() {
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.IniFilename = "build/Atlas/extern/imgui.ini";

    ImGui_ImplOpenGL3_Init("#version 410 core");
}

void OpenGLImGuiSystem::cleanImGuiLayer() {}

}  // namespace Atlas