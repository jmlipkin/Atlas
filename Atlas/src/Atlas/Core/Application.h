#pragma once

#include "Base.h"

#include "Window.h"
#include "LayerStack.h"

#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"

#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Pipeline.h"

// TEMP
#include "Atlas/Renderer/UniformBuffer.h"

namespace Atlas {

class Application {
   public:
    Application();
    virtual ~Application() = default;

    void run();

    void onEvent(Event& event);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    inline Window& getWindow() const { return *m_window; }
    inline static Application& get() { return *s_instance; }

   private:
    bool onWindowClose(WindowCloseEvent& e);

   private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<GraphicsContext> m_context;
    bool m_isRunning = true;

    LayerStack m_layerStack;
    ShaderLibrary m_shaderLib;

    static Application* s_instance;

    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexArray> m_vertexArray;

    std::shared_ptr<Pipeline> m_pipeline;

    // temporary
    protected:
        std::shared_ptr<UniformBuffer> uBuf;
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas