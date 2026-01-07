#include "Application.h"

#include <metal-cpp/Metal.hpp>

#include "Atlas/ImGui/ImGuiLayer.h"
#include "Atlas/Core/Input.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "atpch.h"

namespace Atlas {

Application* Application::s_instance = nullptr;

Application::Application() {
    AT_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = std::shared_ptr<Window>(Window::create());
    m_window->setEventCallback(AT_BIND_EVENT_FN(Application::onEvent));
    m_context = m_window->getGraphicsContext();
    Renderer::init(*m_context);

    m_vertexArray = std::unique_ptr<VertexArray>(VertexArray::create());

    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f};

    m_vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));
    m_vertexArray->addVertexBuffer(m_vertexBuffer);

    uint32_t indices[3] = {0, 1, 2};

    m_indexBuffer = IndexBuffer::create(indices, sizeof(indices));
    m_vertexArray->setIndexBuffer(m_indexBuffer);

    std::string filepath = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/triangle.metallib";
    m_shaderLib.load("Triangle Shader", filepath);

    BufferLayout layout = {
        {"a_Position", ShaderDataType::Float3},
        {"a_Color", ShaderDataType::Float4}};

    PipelineSpecification test_triangle;
    test_triangle.name = "Test Pipeline";
    test_triangle.shader = m_shaderLib.get("Triangle Shader");
    test_triangle.layout = layout;

    m_pipeline = Pipeline::create(test_triangle);

    AT_CORE_INFO("Engine initialization complete!");
}

void Application::run() {
    RenderCommand::setClearColor({0.15f, 0.15f, 0.15f, 1.0f});

    while (m_isRunning) {
        m_window->onUpdate();

        RenderCommand::beginFrame();

        Renderer::submit(*m_pipeline, m_vertexArray);

        for (Layer* l : m_layerStack) {
            l->onUpdate();
        }

        RenderCommand::endFrame();
    }
}

void Application::onEvent(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<WindowCloseEvent>(AT_BIND_EVENT_FN(Application::onWindowClose));

    for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
        (*--it)->onEvent(event);
        if (event.isHandled)
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