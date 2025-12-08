#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"

#include <glad/glad.h>
// #include <GLFW/glfw3.h>
#include "Atlas/Input.h"

namespace Atlas {

    Application* Application::s_instance = nullptr;

    Application::Application() {
        AT_CORE_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::create());
        m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        float vertices[3 * 3] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        m_vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        uint32_t indices[3] = {0, 1, 2};

        m_indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices)));

        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_shader.reset(new Shader(vertexSrc, fragmentSrc));
    }

    Application::~Application() {}

    void Application::run() {

        while (m_isRunning) {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_shader->bind();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

            for (Layer* l : m_layerStack) {
                l->onUpdate();
            }

            m_window->onUpdate();
        }
    }

    void Application::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<WindowCloseEvent>(AT_BIND_EVENT_FN(Application::onWindowClose));

        for (auto it = m_layerStack.end(); it != m_layerStack.begin(); ) {
            (*--it)->onEvent(event);
            if(event.isHandled)
                break;
        }
    }

    void Application::pushLayer(Layer* layer) {
        m_layerStack.pushLayer(layer);
    }

    void Application::pushOverlay(Layer* overlay) {
        m_layerStack.pushOverlay(overlay);
    }

    bool Application::onWindowClose(WindowCloseEvent& e) {
        m_isRunning = false;
        return true;
    }
    }  // namespace Atlas