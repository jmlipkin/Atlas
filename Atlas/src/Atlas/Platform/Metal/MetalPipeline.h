#pragma once

#include <memory>
#include <metal-cpp/Metal.hpp>

#include "Atlas/Platform/Metal/MetalFramebuffer.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Framebuffer.h"
#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {

class MetalPipeline : public Pipeline {
   public:
    MetalPipeline(const PipelineSpecification& specs);
    ~MetalPipeline() override;

	virtual void attachFramebuffer(std::shared_ptr<Framebuffer> framebuffer) override;
    virtual void setLayout(const BufferLayout& layout) override;

    virtual const std::string& getName() const override { return m_name; }

    MTL::RenderPipelineState* getMTLPSO() const { return m_pipelineState; }
    MTL::DepthStencilState* getDepthStencilState() const { return m_depthStencilState; }
    MTL::RenderPipelineDescriptor* getMTLPipeDesc() const { return m_pipelineDescriptor; }

   private:
    void attachLayout();

   private:
    MTL::RenderPipelineState* m_pipelineState;
    MTL::DepthStencilState* m_depthStencilState;
    MTL::RenderPipelineDescriptor* m_pipelineDescriptor;
	MTL::VertexDescriptor* m_vertexDescriptor;

    std::string m_name;
    std::shared_ptr<Framebuffer> m_framebuffer;
};
}  // namespace Atlas