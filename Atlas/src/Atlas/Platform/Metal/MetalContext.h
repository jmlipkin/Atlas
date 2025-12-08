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
#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalContext : public GraphicsContext {
   public:
    MetalContext(GLFWwindow* window);
    virtual void init() override;
    virtual void swapBuffers() override;

    MTL::Device* getMTLDevice() const override { return m_device; }

   private:
    void initDevice();
    void initWindow();

   private:
     GLFWwindow* m_window;
     MTL::Device* m_device;
     void* metalLayer;
};
}  // namespace Atlas