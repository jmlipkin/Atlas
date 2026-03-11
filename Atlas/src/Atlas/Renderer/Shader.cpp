#include "Shader.h"

#include "Atlas/Platform/Metal/MetalShader.h"
#include "Atlas/Platform/OpenGL/OpenGLShader.h"
#include "RendererAPI.h"
#include "atpch.h"

namespace Atlas {

std::shared_ptr<Shader> Shader::create(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) {
	switch (RendererAPI::getAPI()) {
		case RendererAPI::API::None: {
			AT_ASSERT(false, "RendererAPI::None is not supported");
		}
		case RendererAPI::API::OpenGL: {
			return std::make_shared<OpenGLShader>(name, vertexFunction, fragmentFunction);
			break;
		}
		case RendererAPI::API::Metal: {
			return std::make_shared<MetalShader>(name, vertexFunction, fragmentFunction);
			break;
		}
	}

	AT_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}

}  // namespace Atlas