#pragma once

#include "Atlas/Renderer/GraphicsContext.h"
#include "Atlas/Events/ApplicationEvent.h"

namespace Atlas {

class OpenGLContext : public GraphicsContext {
   public:
    OpenGLContext(GLFWwindow* window);

    virtual void init() override;
    virtual void swapBuffers() override;

	virtual void onResize(const WindowResizeEvent& e) override;

	virtual uint32_t getWidth() const override { return 0; }
	virtual uint32_t getHeight() const override {return 0; }

   private:
    GLFWwindow* m_window;
};

}  // namespace Atlas