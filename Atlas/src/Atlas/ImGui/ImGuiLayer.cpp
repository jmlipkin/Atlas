#include "atpch.h"
#include "ImGuiLayer.h"

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Atlas/Core/Base.h>
#include <Atlas/Core/Application.h>
#include "Atlas/Events/ApplicationEvent.h"
#include <Atlas/Renderer/Renderer.h>
#include <Atlas/Core/KeyCodes.h>
#include <Atlas/Core/MouseButtonCodes.h>

namespace Atlas {

static GLFWwindow* getNativeWindow() {
	return (GLFWwindow*)Application::get().getWindow().getNativeWindow();
}

ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {
	m_system = ImGuiSystem::create();
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
	dispatcher.dispatch<DisplayChangedEvent>(AT_BIND_EVENT_FN(ImGuiLayer::onDisplayChangedEvent));
}

bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
	ImGuiIO& io						 = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = true;

	return false;
}

bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
	ImGuiIO& io						 = ImGui::GetIO();
	io.MouseDown[e.getMouseButton()] = false;

	return false;
}

bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e) {
	ImGuiIO& io		= ImGui::GetIO();
	io.MousePos		= ImVec2(e.getX(), e.getY());

	return false;
}

bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e) {
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += e.getXOffset();
	io.MouseWheel += e.getYOffset();

	return false;
}

bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e) {
	ImGuiIO& io	 = ImGui::GetIO();
	ImGuiKey key = keyCodeToImGuiKey(e.getKeyCode());
	io.AddKeyEvent(key, true);

	// TODO: Add modifier key logic

	return false;
}

bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e) {
	ImGuiIO& io	 = ImGui::GetIO();
	ImGuiKey key = keyCodeToImGuiKey(e.getKeyCode());
	io.AddKeyEvent(key, false);

	return false;
}

bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
	ImGuiIO& io		 = ImGui::GetIO();
	int		 keycode = e.getKeyCode();
	if (keycode > 0 && keycode < 0x10000)
		io.AddInputCharacter((unsigned short)keycode);

	return false;
}

bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e) {
	ImGuiIO& io = ImGui::GetIO();

	GLFWwindow* window = getNativeWindow();
	int			winWidth, winHeight, fbWidth, fbHeight;
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

	io.DisplaySize			   = ImVec2((float)winWidth, (float)winHeight);
	io.DisplayFramebufferScale = ImVec2((float)fbWidth / winWidth, (float)fbHeight / winHeight);

	return false;
}

bool ImGuiLayer::onDisplayChangedEvent(DisplayChangedEvent& e) {
	m_system->rescale(e.getScale());

	return false;
}

}  // namespace Atlas