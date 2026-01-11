#include "MetalShader.h"

#include "atpch.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

namespace Atlas {

MetalShader::MetalShader(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) {
    AT_PROFILE_FUNCTION();

    m_name = name;
    m_vertexShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(vertexFunction.c_str(), NS::UTF8StringEncoding));
    m_fragmentShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(fragmentFunction.c_str(), NS::UTF8StringEncoding));
}

MetalShaderLibrary::MetalShaderLibrary(const std::string& filepath) {
    AT_PROFILE_FUNCTION();
    
    m_library = MetalContext::setNewMTLLibrary(filepath);
}

void MetalShaderLibrary::add(std::string name, const std::shared_ptr<Shader>& shader) {
    AT_PROFILE_FUNCTION();

    AT_CORE_ASSERT(!exists(name), "Shader already exists!");
    m_shaders[name] = shader;
}

void MetalShaderLibrary::add(const std::shared_ptr<Shader>& shader) {
    AT_PROFILE_FUNCTION();

    const std::string& name = shader->getName();
    AT_ASSERT(!name.empty(), "Shader must have a name");
    add(name, shader);
}

std::shared_ptr<Shader> MetalShaderLibrary::load(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) {
    AT_PROFILE_FUNCTION();

    std::shared_ptr<Shader> shader = Shader::create(name, vertexFunction, fragmentFunction);
    add(name, shader);
    return shader;
}

std::shared_ptr<Shader> MetalShaderLibrary::get(const std::string& name) const {
    AT_PROFILE_FUNCTION();

    AT_CORE_ASSERT(exists(name), "Shader does not exist!");
    return m_shaders.at(name);
}

bool MetalShaderLibrary::exists(const std::string& name) const {
    AT_PROFILE_FUNCTION();
    
    return m_shaders.find(name) != m_shaders.end();
}

}  // namespace Atlas