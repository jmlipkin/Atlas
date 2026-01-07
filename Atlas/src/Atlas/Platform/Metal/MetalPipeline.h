#pragma once

#include <metal-cpp/Metal.hpp>

#include "Atlas/Renderer/Pipeline.h"
#include "Atlas/Renderer/Buffer.h"

namespace Atlas {
class MetalPipeline : public Pipeline {
   public:
    MetalPipeline(const PipelineSpecification& specs);
    ~MetalPipeline() override;

    virtual void setLayout(const BufferLayout& layout) override;

    MTL::RenderPipelineState* getMTLPSO() const { return m_pipelineState; }

   private:
    void attachLayout();

   private:
    MTL::RenderPipelineState* m_pipelineState;
    MTL::RenderPipelineDescriptor* m_pipelineDescriptor;
    MTL::VertexDescriptor* m_vertexDescriptor;
};
}  // namespace Atlas