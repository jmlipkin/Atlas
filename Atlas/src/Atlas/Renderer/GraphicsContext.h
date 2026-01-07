#pragma once

#include "Atlas/Events/ApplicationEvent.h"

struct GLFWwindow;

namespace MTL {
class Device;
}

namespace Atlas {

    class GraphicsContext {
        public:
         static std::shared_ptr<GraphicsContext> create(GLFWwindow* window);
         virtual void init() = 0;
         virtual void swapBuffers() = 0;

         virtual void onResize(const WindowResizeEvent& e) = 0;
    };

}  // namespace Atlas