#include "atpch.h"
#include "OpenGLImGuiLayer.h"

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Atlas/Core.h>
#include <Atlas/Application.h>
#include <Atlas/KeyCodes.h>
#include <Atlas/MouseButtonCodes.h>
#include <Atlas/Platform/OpenGL/ImGuiOpenGLRenderer.h>

namespace Atlas {

    OpenGLImGuiLayer::OpenGLImGuiLayer() : ImGuiLayer() {

    }

    OpenGLImGuiLayer::~OpenGLImGuiLayer() = default;

    void OpenGLImGuiLayer::onAttach() {
        ImGuiContext *ctx = ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.IniFilename = "build/Atlas/extern/imgui.ini";

        ImGui_ImplOpenGL3_Init("#version 410 core");
    }

    void OpenGLImGuiLayer::onDetach() {

    }

    void OpenGLImGuiLayer::onUpdate() {
        ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(app.getWindow().getNativeWindow()), &fbWidth, &fbHeight);
        io.DisplaySize = ImVec2((float)fbWidth, (float)fbHeight);


		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

}  // namespace Atlas