#pragma once

#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Renderer/GraphicsContext.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

  struct MetalContextData {
      MTL::Device* device = nullptr;
      GLFWwindow* glfwWindow = nullptr;
      CA::MetalLayer* layer = nullptr;

      MTL::CommandQueue* commandQueue = nullptr;
      MTL::CommandBuffer* commandBuffer = nullptr;
      
      CA::MetalDrawable* currentDrawable = nullptr;
      MTL::RenderPassDescriptor* passDesc = nullptr;
  };

  class MetalContext : public GraphicsContext {
     public:
      MetalContext(GLFWwindow* window);
      virtual void init() override;
      virtual void swapBuffers() override {}

      static MetalContextData* getMTLData() { return s_MTLData; }

     private:
      void initDevice();
      void initWindow();

     private:
      static MetalContextData* s_MTLData;
  };
}  // namespace Atlas