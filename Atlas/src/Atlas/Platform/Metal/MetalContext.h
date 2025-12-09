#pragma once

#include "Atlas/Renderer/GraphicsContext.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalContext : public GraphicsContext {
   public:
    MetalContext(GLFWwindow* window);
    virtual void init() override;
    virtual void swapBuffers() override;

    static MTL::Device* getMTLDevice() { return s_device; }
    static CA::MetalLayer* getMTLLayer() { return s_metalLayer; }

   private:
    void initDevice();
    void initWindow();

   private:
     GLFWwindow* m_window;
     static MTL::Device* s_device;
     static CA::MetalLayer* s_metalLayer;
};
}  // namespace Atlas