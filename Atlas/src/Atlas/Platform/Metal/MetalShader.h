#pragma once

#include "Atlas/Renderer/Shader.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalShader : public Shader {
   public:
    MetalShader(const std::string& filepath);
    MetalShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
    virtual ~MetalShader();

    virtual void* getVertexShader() const override { return m_vertexShader; }
    virtual void* getFragmentShader() const override { return m_fragmentShader; }

    virtual const std::string& getName() const override { return m_name; }

   private:
    std::string m_name;

    MTL::Function* m_vertexShader;
    MTL::Function* m_fragmentShader;
};

}  // namespace Atlas