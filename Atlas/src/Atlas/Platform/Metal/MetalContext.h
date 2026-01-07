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

      CA::MetalDrawable* getNextDrawable() { return m_layer->nextDrawable(); }
      MTL::Texture* getMSAATexture() const { return msaaRenderTargetTexture; }
      MTL::Texture* getDepthTexture() const { return depthTexture; }

      CA::MetalLayer* getMTLLayer() const { return m_layer; }

      static MTL::Library* setNewMTLLibrary(const std::string& filepath);
      
      static MTL::Device* getMTLDevice() { return s_device; }
      static MTL::Library* getMTLLibrary() { return s_library; }

      virtual void onResize(const WindowResizeEvent& e) override;

     private:
      void initDevice();
      void initWindow();

      void createDepthAndMSAATextures();

     private:
      static MTL::Device* s_device;
      static MTL::Library* s_library;

      CA::MetalLayer* m_layer;
      GLFWwindow* m_window;

      MTL::Texture* msaaRenderTargetTexture = nullptr;
      MTL::Texture* depthTexture;
      int sampleCount = 4;
  };
}  // namespace Atlas