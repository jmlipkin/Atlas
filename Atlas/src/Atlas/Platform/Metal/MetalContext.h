// #pragma once

// #include "Atlas/Renderer/GraphicsContext.h"

// namespace MTL {
// class Device;
// }
// namespace Atlas {

// class MetalContext : public GraphicsContext {
//    public:
//     MetalContext(GLFWwindow* window);
//     virtual void init() override;
//     virtual void swapBuffers() override;

//     private:
//      void initDevice();
//      void initWindow();

//     private:
//     GLFWwindow* m_window;
//      MTL::Device* m_device;
//      void* metalLayer;
// };
// }  // namespace Atlas




#pragma once

#include "Atlas/Renderer/GraphicsContext.h"

class MTLDevice;
namespace Atlas {

class MetalContext : public GraphicsContext {
   public:
    MetalContext(GLFWwindow* window);
    virtual void init() override;
    virtual void swapBuffers() override;

    private:
     void initDevice();
     void initWindow();

    private:
    GLFWwindow* m_window;
     void* m_device;
     void* metalLayer;
};
}  // namespace Atlas