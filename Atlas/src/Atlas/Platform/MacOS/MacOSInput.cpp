#include "MacOSInput.h"

#include <GLFW/glfw3.h>

#include "Atlas/Application.h"
#include "atpch.h"

namespace Atlas {

Input* Input::s_instance = new MacOSInput();

bool MacOSInput::isKeyPressedImpl(int keycode) {
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool MacOSInput::isMouseButtonPressedImpl(int button) {
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> MacOSInput::getMousePositionImpl() {
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    return {(float)xPos, (float)yPos};
}

float MacOSInput::getMouseXImpl() {
    auto [x, y] = getMousePositionImpl();
    return x;
}

float MacOSInput::getMouseYImpl() {
    auto [x, y] = getMousePositionImpl();
    return y;
}

}  // namespace Atlas