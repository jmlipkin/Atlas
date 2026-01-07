#pragma once

#include <Atlas/Renderer/GraphicsContext.h>
#include <Atlas/Core/Window.h>
#include <GLFW/glfw3.h>

#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MacOSWindow : public Window {
   public:
    MacOSWindow(const WindowProperties& props);
    virtual ~MacOSWindow();

    void onUpdate() override;

    inline unsigned int getWidth() const override { return m_data.width; }
    inline unsigned int getHeight() const override { return m_data.height; }
    inline void* getNativeWindow() const override { return m_window; }
    inline std::shared_ptr<GraphicsContext> getGraphicsContext() const override { return m_data.context; }

    // Window attributes
    inline void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
    void setVSync(bool enabled) override;
    bool isVSync() const override;

   private:
    virtual void init(const WindowProperties& props);
    virtual void shutdown();

   private:
    GLFWwindow* m_window;
    NS::AutoreleasePool* m_pool;

    struct WindowData {
        std::string title;
        unsigned int width, height;
        std::shared_ptr<GraphicsContext> context;
        bool VSync;

        EventCallbackFn eventCallback;
    };

    WindowData m_data;
};

}  // namespace Atlas