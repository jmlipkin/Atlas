#pragma once

#include "Atlas/Renderer/GraphicsContext.h"
#include "Atlas/Events/ApplicationEvent.h"

namespace Atlas {

class OpenGLContext : public GraphicsContext {
   public:
    OpenGLContext(GLFWwindow* window);

    virtual void init() override;
    virtual void swapBuffers() override;

    virtual void beginFrame() override {}
    virtual void endFrame() override {}

    virtual void onResize(const WindowResizeEvent& e) override;

   private:
    GLFWwindow* m_window;
};

}  // namespace Atlas