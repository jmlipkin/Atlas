#pragma once

#include "Atlas/Renderer/Shader.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalShader : public Shader {
   public:
    MetalShader(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction);
    virtual ~MetalShader() = default;

    virtual void* getVertexShader() const override { return m_vertexShader; }
    virtual void* getFragmentShader() const override { return m_fragmentShader; }

    virtual const std::string& getName() const override { return m_name; }

   private:
    std::string m_name;

    MTL::Function* m_vertexShader;
    MTL::Function* m_fragmentShader;
};

class MetalShaderLibrary : public ShaderLibrary {
   public:
    MetalShaderLibrary(const std::string& filepath);
    virtual void add(std::string name, const std::shared_ptr<Shader>& shader) override;
    virtual void add(const std::shared_ptr<Shader>& shader) override;

    virtual std::shared_ptr<Shader> load(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) override;
    virtual std::shared_ptr<Shader> get(const std::string& name) const override;

    virtual bool exists(const std::string& name) const override;

   private:
    MTL::Library* m_library;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};

}  // namespace Atlas