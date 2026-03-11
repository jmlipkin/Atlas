#include "atpch.h"
#include "MetalContext.h"
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <Metal/Metal.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "Atlas/AtlasPaths.h"
#include "Atlas/Events/ApplicationEvent.h"

namespace Atlas {

MTL::Device*  MetalContext::s_device  = nullptr;
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

	NS::Error* error = nullptr;

	MTL::Library* library = s_device->newLibrary(NS::String::string(SHADER_LIBRARY_PATH, NS::UTF8StringEncoding), &error);
	if (!library) {
		AT_CORE_ERROR("Failed to create library at {}", SHADER_LIBRARY_PATH);
		exit(-1);
	}
	s_library = library;
}

void MetalContext::initWindow() {
	AT_PROFILE_FUNCTION();

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);

	NSWindow* nsWindow	  = (NSWindow*)glfwGetCocoaWindow(m_window);
	NSView*	  contentView = nsWindow.contentView;

	// Make the view layer-backed
	[contentView setWantsLayer:YES];

	// Create and attach a CAMetalLayer
	CAMetalLayer* nativeLayer = [CAMetalLayer layer];
	contentView.layer		  = nativeLayer;
	// Wrap in metal-cpp
	m_layer = (CA::MetalLayer*)nativeLayer;

	m_layer->setDevice(s_device);
	m_layer->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
	m_layer->setDrawableSize(CGSizeMake(width, height));
	m_layer->setFramebufferOnly(false);
}

MTL::Library* MetalContext::setNewMTLLibrary(const std::string& filepath) {
	AT_PROFILE_FUNCTION();

	NS::Error*	  error	  = nullptr;
	MTL::Library* library = s_device->newLibrary(NS::String::string(filepath.c_str(), NS::UTF8StringEncoding), &error);
	if (!library) {
		AT_CORE_ERROR("Failed to create library at {}", filepath);
		exit(-1);
	}
	s_library = library;
	return library;
}

void MetalContext::onResize(const WindowResizeEvent& e) {
	AT_PROFILE_FUNCTION();

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
	m_layer->setDrawableSize(CGSizeMake(fbWidth, fbHeight));
}
}  // namespace Atlas