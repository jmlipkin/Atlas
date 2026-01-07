#pragma once

#include <metal-cpp/Metal.hpp>

#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {
class MetalPipeline : public Pipeline {
   public:
    MetalPipeline(const PipelineSpecification& specs);
    ~MetalPipeline() override;

    virtual void setLayout(const BufferLayout& layout) override;

    virtual void setBool(const std::string& name, const bool value) override;
    virtual void setInt(const std::string& name, const int value) override;
    virtual void setFloat(const std::string& name, const float value) override;
    virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void setMat3(const std::string& name, const glm::mat3& value) override;
    virtual void setMat4(const std::string& name, const glm::mat4& value) override;

    MTL::RenderPipelineState* getMTLPSO() const { return m_pipelineState; }

   private:
    void attachLayout();

   private:
    MTL::RenderPipelineState* m_pipelineState;
    MTL::RenderPipelineDescriptor* m_pipelineDescriptor;
    MTL::VertexDescriptor* m_vertexDescriptor;
};
}  // namespace Atlas