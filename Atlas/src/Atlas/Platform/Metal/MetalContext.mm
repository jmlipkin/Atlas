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

    MetalContextData* MetalContext::s_MTLData = nullptr;

    MetalContext::MetalContext(GLFWwindow* window) {
        AT_CORE_ASSERT(window, "Window handle is null!");
        AT_CORE_TRACE("Created MetalContext (constructor)");
        s_MTLData = new MetalContextData();
        s_MTLData->glfwWindow = window;
    }

    void MetalContext::init() { 
        initDevice();
        initWindow();
        AT_CORE_TRACE("MetalContext initialized!");
    }

    void MetalContext::initDevice() {
        s_MTLData->device = MTL::CreateSystemDefaultDevice();
        s_MTLData->commandQueue = s_MTLData->device->newCommandQueue();

        // An assert here means MetalContext::initDevice() could not create system default device.
        AT_CORE_ASSERT(s_MTLData->device, "MetalContext device is null!");
    }

    void MetalContext::initWindow() {
        int width, height;
        glfwGetFramebufferSize(s_MTLData->glfwWindow, &width, &height);

        NSWindow* nsWindow = (NSWindow*)glfwGetCocoaWindow(s_MTLData->glfwWindow);
        NSView* contentView = nsWindow.contentView;

        // Make the view layer-backed
        [contentView setWantsLayer:YES];

        // Create and attach a CAMetalLayer
        CAMetalLayer* nativeLayer = [CAMetalLayer layer];
        contentView.layer = nativeLayer;

        // Wrap in metal-cpp
        s_MTLData->layer = (CA::MetalLayer*)nativeLayer;
        
        s_MTLData->layer->setDevice(s_MTLData->device);
        s_MTLData->layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        s_MTLData->layer->setDrawableSize(CGSizeMake(width, height));
        s_MTLData->layer->setFramebufferOnly(true);
    }
} // namespace Atlas