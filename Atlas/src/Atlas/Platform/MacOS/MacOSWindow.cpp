#include "atpch.h"
#include "MacOSWindow.h"

namespace Atlas {

static bool s_GLFWInitialized = false;

Window* Window::create(const WindowProperties& props) {
    return new MacOSWindow(props);
}

MacOSWindow::MacOSWindow(const WindowProperties& props) {
    init(props);
}

MacOSWindow::~MacOSWindow() {
    shutdown();
}

void MacOSWindow::init(const WindowProperties& props) {
    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    AT_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

    if(!s_GLFWInitialized) {
        int success = glfwInit();
        AT_CORE_ASSERT(success, "Could not initialized GLFW!");

        s_GLFWInitialized = true;
    }

    m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, &m_data);
    setVSync(true);

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    AT_CORE_ERROR("Failed to initialize GLAD!");
}

void MacOSWindow::shutdown() {
    glfwDestroyWindow(m_window);
}

void MacOSWindow::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void MacOSWindow::setVSync(bool enabled) {
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_data.VSync = enabled;
}

bool MacOSWindow::isVSync() const {
    return m_data.VSync;
}
}