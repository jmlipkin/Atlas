#include "OpenGLContext.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "atpch.h"

namespace Atlas {

OpenGLContext::OpenGLContext(GLFWwindow* window) : m_window(window) {
    AT_CORE_ASSERT(window, "Window handle is null!");
}

void OpenGLContext::init() {
    glfwMakeContextCurrent(m_window);

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        AT_CORE_ERROR("Failed to initialize GLAD!");

    AT_CORE_INFO("Initialized GLAD!");
}

void OpenGLContext::swapBuffers() {
}

}  // namespace Atlas