#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"
#include "Atlas/Input.h"
#include "Atlas/Renderer/Renderer.h"
#include "Atlas/ImGui/ImGuiLayer.h"
namespace Atlas {

    Application* Application::s_instance = nullptr;

    Application::Application() {
        AT_CORE_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::create());
        m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));

        m_vertexArray = std::unique_ptr<VertexArray>(VertexArray::create());
        m_vertexArray->bind();

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
        m_vertexArray->addVertexBuffer(m_vertexBuffer);

        uint32_t indices[3] = {0, 1, 2};

        m_indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices)));
        m_vertexArray->setIndexBuffer(m_indexBuffer);
        
        m_squareVA.reset(VertexArray::create());

        float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
            0.75f, -0.75f, 0.0f,
            0.75f, 0.75f, 0.0f,
            -0.75f, 0.75f, 0.0f};

        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::create(squareVertices, sizeof(squareVertices)));
        squareVB->setLayout({{"a_Position", ShaderDataType::Float3}});
        m_squareVA->addVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_squareVA->setIndexBuffer(squareIB);

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

        std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_blueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
    }

    void Application::run() {

        while (m_isRunning) {
            RenderCommand::setClearColor({0.15f, 0.15f, 0.15f, 1.0f});
            RenderCommand::clear();

            Renderer::beginScene();

            m_blueShader->bind();

            Renderer::submit(m_squareVA);

            m_shader->bind();
            m_vertexArray->bind();
            Renderer::submit(m_vertexArray);

            Renderer::endScene();

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