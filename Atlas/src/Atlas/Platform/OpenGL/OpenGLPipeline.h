#pragma once

#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {
class OpenGLPipeline : public Pipeline {
   public:
    OpenGLPipeline(const PipelineSpecification& specs) : m_name(specs.name) {}

    virtual void setLayout(const BufferLayout& layout) override {}
    virtual void attachFramebuffer(std::shared_ptr<Framebuffer> framebuffer) override {}

    virtual const std::string& getName() const override { return m_name; }
	
   private:
    void attachLayout() {}

    std::string m_name;
};
}  // namespace Atlas