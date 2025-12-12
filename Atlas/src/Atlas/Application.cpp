#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"
#include "Atlas/Input.h"
#include "Atlas/Renderer/Renderer.h"
#include "Atlas/ImGui/ImGuiLayer.h"

#include <metal-cpp/Metal.hpp>
namespace Atlas {

    Application* Application::s_instance = nullptr;

    Application::Application() {
        AT_CORE_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        m_window = std::unique_ptr<Window>(Window::create());
        m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));

        Renderer::init();

        // m_vertexArray = std::unique_ptr<VertexArray>(VertexArray::create());
        // m_vertexArray->bind();

        // float vertices[3 * 7] = {
        //     -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        //     0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        //     0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        // };

        // m_vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

        // {
        //     BufferLayout layout = {
        //         {"a_Position", ShaderDataType::Float3},
        //         {"a_Color", ShaderDataType::Float4}
        //     };

        //     m_vertexBuffer->setLayout(layout);
        // }
        // m_vertexArray->addVertexBuffer(m_vertexBuffer);

        // uint32_t indices[3] = {0, 1, 2};

        // m_indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices)));
        // m_vertexArray->setIndexBuffer(m_indexBuffer);
    }

    void Application::run() {

        while (m_isRunning) {
            NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

            RenderCommand::setClearColor({0.15f, 0.15f, 0.15f, 1.0f});
            RenderCommand::clear();
            
            Renderer::beginScene();

            // m_shader->bind();
            // m_vertexArray->bind();
            // Renderer::submit(m_vertexArray);

            Renderer::endScene();

            for (Layer* l : m_layerStack) {
                l->onUpdate();
            }

            RenderCommand::commit();
            m_window->onUpdate();

            pool->release();
        }
    }

    void Application::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<WindowCloseEvent>(AT_BIND_EVENT_FN(Application::onWindowClose));

        // Temporary
        RenderCommand::onEvent(event);

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