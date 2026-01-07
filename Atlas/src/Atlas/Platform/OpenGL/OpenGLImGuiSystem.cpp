#include "OpenGLImGuiSystem.h"

// #include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui/imgui.h>

#include "atpch.h"

// #include <Atlas/Core/Base.h>
// #include <Atlas/Core/Application.h>
// #include <Atlas/Core/KeyCodes.h>
// #include <Atlas/Core/MouseButtonCodes.h>
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