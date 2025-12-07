#include "atpch.h"
#include "MacOSWindow.h"

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>

namespace Atlas {

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description) {
    AT_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

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

    AT_CORE_TRACE("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

    if(!s_GLFWInitialized) {
        int success = glfwInit();
        AT_CORE_ASSERT(success, "Could not initialize GLFW!");

        AT_CORE_INFO("Initialized GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, &m_data);
    setVSync(true);

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        AT_CORE_ERROR("Failed to initialize GLAD!");

    AT_CORE_INFO("Initialized GLAD!");
    // set GLFW callbacks

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;
        
        WindowResizeEvent event(width, height);
        data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.eventCallback(event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch(action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keycode);
        data.eventCallback(event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        
        switch(action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseScrolledEvent event(xOffset, yOffset);
        data.eventCallback(event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event((float)x, (float)y);
        data.eventCallback(event);
    });
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