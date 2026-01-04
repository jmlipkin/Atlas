#pragma once

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

         virtual void beginFrame() = 0;
         virtual void endFrame() = 0;
    };

}  // namespace Atlas