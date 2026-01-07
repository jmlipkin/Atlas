#include "Shader.h"

#include "Atlas/Platform/Metal/MetalShader.h"
#include "Atlas/Platform/OpenGL/OpenGLShader.h"
#include "RendererAPI.h"
#include "atpch.h"

namespace Atlas {

std::shared_ptr<Shader> Shader::create(const std::string& name, const std::string& filepath) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_ASSERT(false, "RendererAPI::None is not supported");
        }
        case RendererAPI::API::OpenGL: {
            return std::make_shared<OpenGLShader>(name, filepath);
            break;
        }
        case RendererAPI::API::Metal: {
            return std::make_shared<MetalShader>(name, filepath);
            break;
        }
    }

    AT_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
}

std::shared_ptr<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc) {
    switch (RendererAPI::getAPI()) {
        case RendererAPI::API::None: {
            AT_ASSERT(false, "RendererAPI::None is not supported");
        }
        case RendererAPI::API::OpenGL: {
            return std::make_shared<OpenGLShader>(name, vertexSrc, fragSrc);
            break;
        }
        case RendererAPI::API::Metal: {
            return std::make_shared<MetalShader>(name, vertexSrc, fragSrc);
            break;
        }
    }

    AT_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
}

void ShaderLibrary::add(std::string name, const std::shared_ptr<Shader>& shader) {
    AT_CORE_ASSERT(!exists(name), "Shader already exists!");
    m_shaders[name] = shader;
}

void ShaderLibrary::add(const std::shared_ptr<Shader>& shader) {
    const std::string& name = shader->getName();
    add(name, shader);
}

std::shared_ptr<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
    std::shared_ptr<Shader> shader = Shader::create(name, filepath);
    add(name, shader);
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::get(const std::string& name) const {
    AT_CORE_ASSERT(exists(name), "Shader does not exist!");
    return m_shaders.at(name);
}

bool ShaderLibrary::exists(const std::string& name) const {
    return m_shaders.find(name) != m_shaders.end();
}

}  // namespace Atlas