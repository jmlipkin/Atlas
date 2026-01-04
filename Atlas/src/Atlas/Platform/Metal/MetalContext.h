#pragma once

#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Renderer/GraphicsContext.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

  class MetalContext : public GraphicsContext {
     public:
      MetalContext(GLFWwindow* window);
      virtual void init() override;
      virtual void swapBuffers() override {}

      CA::MetalLayer* getMTLLayer() const { return m_layer; }
      
      static MTL::Library* setNewMTLLibrary(const std::string& filepath);

      static void setEncoder(MTL::RenderCommandEncoder* encoder) { s_encoder = encoder;  }

      static MTL::Device* getMTLDevice() { return s_device; }
      static MTL::Library* getMTLLibrary() { return s_library; }
      static MTL::CommandQueue* getMTLQueue() { return s_commandQueue; }
      static MTL::CommandBuffer* getCommandBuffer() { return s_commandBuffer; }
      static MTL::RenderPassDescriptor* getPassDesc() { return s_passDesc; }
      static MTL::RenderCommandEncoder* getEncoder() { return s_encoder; }
      static CA::MetalDrawable* getMTLDrawable() { return s_drawable; }

      virtual void beginFrame() override;
      virtual void endFrame() override;

     private:
      void initDevice();
      void initWindow();

     private:
      static MTL::Device* s_device;
      static MTL::Library* s_library;
      static MTL::CommandQueue* s_commandQueue;
      static MTL::RenderPassDescriptor* s_passDesc;
      static MTL::RenderCommandEncoder* s_encoder;
      static MTL::CommandBuffer* s_commandBuffer;
      static CA::MetalDrawable* s_drawable;

      CA::MetalLayer* m_layer;
      NS::AutoreleasePool* m_pool;

      GLFWwindow* m_window;
  };
}  // namespace Atlas