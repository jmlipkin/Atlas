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

      static MTL::Device* getMTLDevice() { return s_device; }
      static MTL::Library* getMTLLibrary() { return s_library; }

      virtual void beginFrame() override;
      virtual void endFrame() override;

      CA::MetalDrawable* getNextDrawable() { return m_layer->nextDrawable(); }

      virtual void onResize(const WindowResizeEvent& e) override;

     private:
      void initDevice();
      void initWindow();

     private:
      static MTL::Device* s_device;
      static MTL::Library* s_library;
      
      CA::MetalLayer* m_layer;
      GLFWwindow* m_window;
  };
}  // namespace Atlas