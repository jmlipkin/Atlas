#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Atlas/Window.h>
#include <Atlas/Renderer/GraphicsContext.h>

namespace Atlas {

    class MacOSWindow : public Window {
    public:
     MacOSWindow(const WindowProperties& props);
     virtual ~MacOSWindow();

     void onUpdate() override;

     inline unsigned int getWidth() const override { return m_data.width; }
     inline unsigned int getHeight() const override { return m_data.height; }
     inline void* getNativeWindow() const override { return m_window; }

     // Window attributes
     inline void setEventCallback(const EventCallbackFn& callback) override { m_data.eventCallback = callback; }
     void setVSync(bool enabled) override;
     bool isVSync() const override;

     private:
      virtual void init(const WindowProperties& props);
      virtual void shutdown();
        private:
         GLFWwindow* m_window;
         GraphicsContext* m_context;

         struct WindowData {
             std::string title;
             unsigned int width, height;
             bool VSync;

             EventCallbackFn eventCallback;
         };

         WindowData m_data;
    };

}  // namespace Atlas