#include "Application.h"

#include <metal-cpp/Metal.hpp>

#include "Atlas/Core/Input.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/ImGui/ImGuiLayer.h"
#include "Atlas/Renderer/Renderer.h"
#include "atpch.h"

// TEMP
#include "glm/glm.hpp"

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

    float vertices[4 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };

    m_vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));
    m_vertexArray->addVertexBuffer(m_vertexBuffer);

    uint32_t indices[6] = {0, 1, 2, 0, 2, 3};

    m_indexBuffer = IndexBuffer::create(indices, sizeof(indices));
    m_vertexArray->setIndexBuffer(m_indexBuffer);

    std::string filepath = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/shaders.metallib";
    m_shaderLib = ShaderLibrary::create(filepath);
    m_shaderLib->load("Triangle Shader", "triangleVertexFunction", "triangleFragmentFunction");

    BufferLayout layout = {
        {"a_Position", ShaderDataType::Float3},
        {"a_Color", ShaderDataType::Float4}};

    PipelineSpecification test_triangle;
    test_triangle.name = "Test Pipeline";
    test_triangle.shader = m_shaderLib->get("Triangle Shader");
    test_triangle.layout = layout;

    UniformBufferLayout uniform = {
        {"position", 0, glm::ivec3{0, 0, 0}},
        {"bigData", 1, glm::mat4{0.2f}},
        {"color", 2, glm::vec4{0.2f, 0.1f, 0.8f, 1.0f}}};

    uBuf = UniformBuffer::create(test_triangle, uniform, 1);
    m_pipeline = Pipeline::create(test_triangle);

    AT_CORE_INFO("Engine initialization complete!");
}

void Application::run() {
    RenderCommand::setClearColor({0.15f, 0.15f, 0.15f, 1.0f});

    using Clock = std::chrono::high_resolution_clock;  // or system_clock
    using Duration = std::chrono::duration<float>;     // float seconds

    Clock::time_point last_loop = Clock::now();
    Duration delta_time(0.0f);
    float time = 0.0f;

    while (m_isRunning) {
        m_window->onUpdate();

        auto now = Clock::now();
        delta_time = now - last_loop;
        last_loop = now;
        time += delta_time.count();

        RenderCommand::beginFrame();

        float a = (sin(2 * time) + 1.0f) / 2.0f;
        // float a = 0.8f;
        uBuf->setFloat4("color", glm::vec4{1.0f, 1.0f, 1.0f, a});

        Renderer::submit(*m_pipeline, m_vertexArray, *uBuf);

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