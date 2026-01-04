#pragma once

#include "Atlas/Renderer/GraphicsContext.h"

namespace Atlas {

class OpenGLContext : public GraphicsContext {
   public:
    OpenGLContext(GLFWwindow* window);

    virtual void init() override;
    virtual void swapBuffers() override;

    virtual void beginFrame() override {}
    virtual void endFrame() override {}

   private:
    GLFWwindow* m_window;
};

}  // namespace Atlas