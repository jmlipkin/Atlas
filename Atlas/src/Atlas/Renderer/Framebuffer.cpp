#include "Framebuffer.h"
#include "atpch.h"

#include "Atlas/Platform/Metal/MetalFramebuffer.h"
#include "RendererAPI.h"

namespace Atlas {

std::shared_ptr<Framebuffer> Framebuffer::create(const FramebufferSpecifications &specs) {
	switch (RendererAPI::getAPI()) {
		case RendererAPI::API::None:
			return nullptr;
		case RendererAPI::API::OpenGL:
			return nullptr;
		case RendererAPI::API::Metal:
			return std::make_shared<MetalFramebuffer>(specs);
	}
}

}  // namespace Atlas