#include "atpch.h"
#include "ImGuiLayer.h"

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/glm.hpp>

#include <Atlas/Core/Base.h>
#include <Atlas/Core/Application.h>
#include <Atlas/Renderer/Renderer.h>
#include <Atlas/Core/KeyCodes.h>
#include <Atlas/Core/MouseButtonCodes.h>

namespace Atlas {

    ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {
        m_system = ImGuiSystem::create();
		m_sink = Atlas::Log::getImGuiLogger();
    }

    void ImGuiLayer::onUpdate(DeltaTime dt) {
        GLFWwindow* window = (GLFWwindow*)Application::get().getWindow().getNativeWindow();

        int winWidth, winHeight;
        int fbWidth, fbHeight;

        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		ImGuiIO& io = ImGui::GetIO();
        ImFont* helvetica = io.Fonts->AddFontFromFileTTF("Atlas/assets/helvetica-neue-5/HelveticaNeueLight.otf", 14);
		io.FontDefault = helvetica;
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

        onImGuiRender(dt);
        drawImGuiLogWindow();

        ImGui::Render();
        Renderer::submitImGui();
    }

    void ImGuiLayer::onEvent(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.dispatch<MouseButtonPressedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onMouseButtonPressedEvent));
        dispatcher.dispatch<MouseButtonReleasedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
        dispatcher.dispatch<MouseMovedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
        dispatcher.dispatch<MouseScrolledEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
        dispatcher.dispatch<KeyPressedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
        dispatcher.dispatch<KeyReleasedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
        dispatcher.dispatch<KeyTypedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
        dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));

    }

    bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = true;

        return false;
    }

    bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = false;

        return false;
    }

    bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

        return false;
    }

    bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.getXOffset();
		io.MouseWheel += e.getYOffset();

		return false;
    }

    bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey key = keyCodeToImGuiKey(e.getKeyCode());
        io.AddKeyEvent(key, true);

        // TODO: Add modifier key logic

        return false;
    }

    bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey key = keyCodeToImGuiKey(e.getKeyCode());
        io.AddKeyEvent(key, false);

        return false;
    }

    bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
		int keycode = e.getKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

        return false;
    }

    bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

        return false;
    }

    void ImGuiLayer::drawImGuiLogWindow()
{
    ImGui::Begin("Log");

    if (ImGui::Button("Clear"))
        m_sink->clear();

    ImGui::Separator();

    ImGui::BeginChild("Scrolling");

    for (const auto& message : m_sink->getMessages())
    {
        ImVec4 color;
        color.x = message.color.x;
        color.y = message.color.y;
        color.z = message.color.z;
        color.w = message.color.w;
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(message.text.c_str());
        ImGui::PopStyleColor();
    }

    if (m_sink->scrollsToBottom())
    {
        ImGui::SetScrollHereY(1.0f);
        m_sink->clearScrollFlag();
    }

    ImGui::EndChild();
    ImGui::End();
}


}  // namespace Atlas