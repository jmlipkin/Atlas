#pragma once

#include "Atlas/Renderer/Shader.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalShader : public Shader {
   public:
    MetalShader(const std::string& filepath);
    MetalShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
    virtual ~MetalShader();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual const std::string& getName() const override { return m_name; }

   private:
    MTL::RenderPipelineState* m_pipelineState;
    std::string m_name;
};

}  // namespace Atlas