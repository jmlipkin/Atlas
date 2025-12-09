#include "atpch.h"
#include "MetalContext.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace Atlas {

    MTL::Device* MetalContext::s_device = nullptr;
    CA::MetalLayer* MetalContext::s_metalLayer = nullptr;

    MetalContext::MetalContext(GLFWwindow* window) : m_window(window) {
        AT_CORE_ASSERT(window, "Window handle is null!");
        AT_CORE_TRACE("Created MetalContext (constructor)");
    }

    void MetalContext::init() { 
        initDevice();
        initWindow();
        AT_CORE_TRACE("MetalContext initialized!");
    }

    void MetalContext::swapBuffers() {
    }

    void MetalContext::initDevice() {
        s_device = MTL::CreateSystemDefaultDevice();

        // An assert here means MetalContext::initDevice() could not create system default device.
        AT_CORE_ASSERT(s_device, "MetalContext device is null!");
    }

    void MetalContext::initWindow() {
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
        s_metalLayer = (CA::MetalLayer*)nativeLayer;

        // CAMetalLayer* nativeLayer = (CAMetalLayer*)contentView.layer;
        // s_metalLayer = (CA::MetalLayer*)nativeLayer;

        // metalLayer = (CA::MetalLayer*)glfwGetCocoaWindow(m_window).contentView.layer;
        // CAMetalLayer* metalLayer = [CAMetalLayer layer];
        
        s_metalLayer->setDevice(s_device);
        s_metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        s_metalLayer->setDrawableSize(CGSizeMake(width, height));
        s_metalLayer->setFramebufferOnly(true);
        // metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        // metalLayer.drawableSize = CGSizeMake(width, height);
        // [metalWindow.contentView setLayer:metalLayer];
        // [metalWindow.contentView setWantsLayer:YES];
    }
} // namespace Atlas