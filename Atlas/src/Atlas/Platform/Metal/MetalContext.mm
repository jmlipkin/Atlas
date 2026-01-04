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
    MTL::CommandQueue* MetalContext::s_commandQueue = nullptr;
    MTL::RenderPassDescriptor* MetalContext::s_passDesc = nullptr;
    MTL::RenderCommandEncoder* MetalContext::s_encoder = nullptr;
    MTL::CommandBuffer* MetalContext::s_commandBuffer = nullptr;
    CA::MetalDrawable* MetalContext::s_drawable = nullptr;

    MetalContext::MetalContext(GLFWwindow* window) : m_window(window) {
        AT_CORE_ASSERT(window, "Window handle is null!");
        AT_CORE_TRACE("Created MetalContext (constructor)");
    }

    void MetalContext::init() { 
        initDevice();
        initWindow();
        AT_CORE_TRACE("MetalContext initialized!");
    }

    void MetalContext::initDevice() {

        s_device = MTL::CreateSystemDefaultDevice();
        // An assert here means MetalContext::initDevice() could not create system default device.
        AT_CORE_ASSERT(s_device, "MetalContext device is null!");

        s_library = s_device->newDefaultLibrary();
        s_commandQueue = s_device->newCommandQueue();
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
        m_layer = (CA::MetalLayer*)nativeLayer;
        
        m_layer->setDevice(s_device);
        m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        m_layer->setDrawableSize(CGSizeMake(width, height));
        m_layer->setFramebufferOnly(true);
    }

    MTL::Library* MetalContext::setNewMTLLibrary(const std::string& filepath) {
        NS::Error* error = nullptr;
        MTL::Library* library = getMTLDevice()->newLibrary(NS::String::string(filepath.c_str(), NS::UTF8StringEncoding), &error);
        if (!library) {
            AT_CORE_ERROR("Failed to create library");
            exit(-1);
        }
        s_library = library;
        return library;
    }

    void MetalContext::beginFrame() {
        m_pool = NS::AutoreleasePool::alloc()->init();
        s_drawable = m_layer->nextDrawable();
        if (!s_drawable) {
            AT_WARN("CA::MetalDrawable* not drawable (MetalContext)");
            return;
        }
        s_passDesc = MTL::RenderPassDescriptor::alloc()->init();

        s_commandBuffer = s_commandQueue->commandBuffer();

    }

    void MetalContext::endFrame() {
        s_passDesc->release();
        m_pool->release();
    }
} // namespace Atlas