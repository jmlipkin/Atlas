#include "MacOSWindow.h"

#include <Atlas/Events/ApplicationEvent.h>
#include <Atlas/Events/KeyEvent.h>
#include <Atlas/Events/MouseEvent.h>

#include <metal-cpp/Metal.hpp>

#include "Atlas/Core/Input.h"
#include "Atlas/Core/KeyCodes.h"
#include "GLFW/glfw3.h"
#include "atpch.h"
#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas {

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description) {
    AT_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::create(const WindowProperties& props) {
    AT_PROFILE_FUNCTION();
    return new MacOSWindow(props);
}

MacOSWindow::MacOSWindow(const WindowProperties& props) {
    init(props);
}

MacOSWindow::~MacOSWindow() {
    shutdown();
}

void MacOSWindow::init(const WindowProperties& props) {
    AT_PROFILE_FUNCTION();

    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    AT_CORE_TRACE("Creating window \"{0}\" ({1}x{2})", props.title, props.width, props.height);

    if (!s_GLFWInitialized) {
        AT_PROFILE_SCOPE("Initialize GLFW");

        int success = glfwInit();
        AT_CORE_ASSERT(success, "Could not initialize GLFW!");

        AT_CORE_TRACE("GLFW initialized");
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    {
        AT_PROFILE_SCOPE("GLFW Window Hints");

        if (RendererAPI::getAPI() == RendererAPI::API::OpenGL) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        } else if (RendererAPI::getAPI() == RendererAPI::API::Metal) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        glfwWindowHint(GLFW_MAXIMIZED, props.openMaximized);
    }

    {
        AT_PROFILE_SCOPE("GLFW create window");

        m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
}

    m_data.context = GraphicsContext::create(m_window);
    m_data.context->init();

    glfwSetWindowUserPointer(m_window, &m_data);
    // cannot call swap interval using metal
    // setVSync(true);

    // set GLFW callbacks

    {
        AT_PROFILE_SCOPE("Set GLFW Callbacks");

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

		glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (focused) {
				WindowFocusEvent event;
				data.eventCallback(event);
			} else {
				WindowLostFocusEvent event;
				data.eventCallback(event);
            }
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
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

            switch (action) {
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
}

void MacOSWindow::shutdown() {
    AT_PROFILE_FUNCTION();

    {
        AT_PROFILE_SCOPE("GLFW Destroy window");
        glfwDestroyWindow(m_window);
    }
}

void MacOSWindow::onUpdate() {
    glfwPollEvents();
	m_data.context->swapBuffers();
}

void MacOSWindow::onMinimize() {
	glfwIconifyWindow(m_window);
}

void MacOSWindow::onRestore() {
	glfwRestoreWindow(m_window);
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
}  // namespace Atlas