#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"

#include <glad/glad.h>
// #include <GLFW/glfw3.h>
#include "Atlas/Input.h"

namespace Atlas {

    Application* Application::s_instance = nullptr;

    static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
            case Atlas::ShaderDataType::None:     return GL_NONE;
			case Atlas::ShaderDataType::Float:    return GL_FLOAT;
			case Atlas::ShaderDataType::Float2:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float3:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float4:   return GL_FLOAT;
			case Atlas::ShaderDataType::Mat3:     return GL_FLOAT;
			case Atlas::ShaderDataType::Mat4:     return GL_FLOAT;
			case Atlas::ShaderDataType::Int:      return GL_INT;
			case Atlas::ShaderDataType::Int2:     return GL_INT;
			case Atlas::ShaderDataType::Int3:     return GL_INT;
			case Atlas::ShaderDataType::Int4:     return GL_INT;
			case Atlas::ShaderDataType::Bool:     return GL_BOOL;
		}
		
		AT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

    Application::Application() {
        AT_CORE_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::create());
        m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
            0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        m_vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

        {
            BufferLayout layout = {
                {"a_Position", ShaderDataType::Float3},
                {"a_Color", ShaderDataType::Float4}
            };

            m_vertexBuffer->setLayout(layout);
        }

        uint32_t index = 0;
		const auto& layout = m_vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				shaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset);
			index++;
		}

        uint32_t indices[3] = {0, 1, 2};

        m_indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices)));

        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
            out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
                v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

        std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
            in vec4 v_Color;

			void main()
			{
				color = v_Color;
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