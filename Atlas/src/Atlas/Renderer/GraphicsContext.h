#pragma once

struct GLFWwindow;

namespace MTL {
class Device;
}

namespace Atlas {

    class GraphicsContext {
        public:
         static GraphicsContext* create(GLFWwindow* window);
         virtual void init() = 0;
         virtual void swapBuffers() = 0;
    };

}  // namespace Atlas