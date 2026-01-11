#include "atpch.h"
#include "MetalContext.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "Atlas/Events/ApplicationEvent.h"

namespace Atlas {

    MTL::Device* MetalContext::s_device = nullptr;
    MTL::Library* MetalContext::s_library = nullptr;

    MetalContext::MetalContext(GLFWwindow* window) : m_window(window) {
        AT_CORE_ASSERT(window, "Window handle is null!");
    }

    void MetalContext::init() { 
        AT_PROFILE_FUNCTION();
        
        initDevice();
        initWindow();
        AT_CORE_TRACE("MetalContext initialized");
    }

    void MetalContext::initDevice() {
        AT_PROFILE_FUNCTION();

        s_device = MTL::CreateSystemDefaultDevice();
        // An assert here means MetalContext::initDevice() could not create system default device.
        AT_CORE_ASSERT(s_device, "MetalContext device is null!");

        s_library = s_device->newDefaultLibrary();
    }

    void MetalContext::initWindow() {
        AT_PROFILE_FUNCTION();

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        NSWindow* nsWindow = (NSWindow*)glfwGetCocoaWindow(m_window);
        NSView* contentView = nsWindow.contentView;

        // Make the view layer-backed
        [contentView setWantsLayer:YES];

        // Create and attach a CAMetalLayer
        CAMetalLayer* nativeLayer = [CAMetalLayer layer];
        contentView.layer = nativeLayer;

        // Wrap in metal-cpp
        m_layer = (CA::MetalLayer*)nativeLayer;
        
        m_layer->setDevice(s_device);
        m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        m_layer->setDrawableSize(CGSizeMake(width, height));
        m_layer->setFramebufferOnly(true);

        createDepthAndMSAATextures();
    }

    MTL::Library* MetalContext::setNewMTLLibrary(const std::string& filepath) {
        AT_PROFILE_FUNCTION();

        NS::Error* error = nullptr;
        MTL::Library* library = getMTLDevice()->newLibrary(NS::String::string(filepath.c_str(), NS::UTF8StringEncoding), &error);
        if (!library) {
            AT_CORE_ERROR("Failed to create library");
            exit(-1);
        }
        s_library = library;
        return library;
    }

    void MetalContext::createDepthAndMSAATextures() {
        AT_PROFILE_FUNCTION();
        
        MTL::TextureDescriptor* msaaTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
        msaaTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
        msaaTextureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        msaaTextureDescriptor->setWidth(m_layer->drawableSize().width);
        msaaTextureDescriptor->setHeight(m_layer->drawableSize().height);
        msaaTextureDescriptor->setSampleCount(sampleCount);
        msaaTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);

        msaaRenderTargetTexture = s_device->newTexture(msaaTextureDescriptor);

        MTL::TextureDescriptor* depthTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
        depthTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
        depthTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
        depthTextureDescriptor->setWidth(m_layer->drawableSize().width);
        depthTextureDescriptor->setHeight(m_layer->drawableSize().height);
        depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
        depthTextureDescriptor->setSampleCount(sampleCount);

        depthTexture = s_device->newTexture(depthTextureDescriptor);

        msaaTextureDescriptor->release();
        depthTextureDescriptor->release();
    }

    void MetalContext::onResize(const WindowResizeEvent& e) {
        AT_PROFILE_FUNCTION();
        
        m_layer->setDrawableSize(CGSizeMake(e.getWidth(), e.getHeight()));
        createDepthAndMSAATextures();
    }
} // namespace Atlas