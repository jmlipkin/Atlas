#include "atpch.h"
#include "MetalContext.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <metal-cpp/Metal.hpp>
#include <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace Atlas {
    MetalContext::MetalContext(GLFWwindow* window) : m_window(window) {
        AT_CORE_ASSERT(window, "Window handle is null!");
        AT_CORE_TRACE("Created MetalContext (constructor)");
    }

    void MetalContext::init() { 
        initDevice();
        initWindow();
        AT_CORE_TRACE("MetalContext initialized!");
    }

    void MetalContext::swapBuffers() {}

    void MetalContext::initDevice() {
        m_device = MTL::CreateSystemDefaultDevice();
    }

    void MetalContext::initWindow() {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        NSWindow* metalWindow = glfwGetCocoaWindow(m_window);
        CAMetalLayer* metalLayer = [CAMetalLayer layer];
        metalLayer.device = (id<MTLDevice>)m_device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.drawableSize = CGSizeMake(width, height);
        [metalWindow.contentView setLayer:metalLayer];
        [metalWindow.contentView setWantsLayer:YES];
    }
} // namespace Atlas