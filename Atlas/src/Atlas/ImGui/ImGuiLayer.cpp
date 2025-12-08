#include "atpch.h"
#include "ImGuiLayer.h"

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Atlas/Core.h>
#include <Atlas/Application.h>
#include <Atlas/Renderer/Renderer.h>
#include <Atlas/KeyCodes.h>
#include <Atlas/MouseButtonCodes.h>

#include <Atlas/Platform/OpenGL/OpenGLImGuiLayer.h>
#include <Atlas/Platform/Metal/MetalImGuiLayer.h>

namespace Atlas {

    ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {

    }

    GraphicsContext* ImGuiLayer::m_context = nullptr;

    ImGuiLayer* ImGuiLayer::create() {
        switch (Renderer::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
            }
            case RendererAPI::API::OpenGL: {
                return new OpenGLImGuiLayer();
            }
            case RendererAPI::API::Metal: {
                return new MetalImGuiLayer();
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
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
		glViewport(0, 0, e.getWidth(), e.getHeight());

        return false;
    }

}  // namespace Atlas