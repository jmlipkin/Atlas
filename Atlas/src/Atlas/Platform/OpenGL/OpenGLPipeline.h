#pragma once

#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {
class OpenGLPipeline : public Pipeline {
   public:
    OpenGLPipeline(const PipelineSpecification& specs) {}

    virtual void setLayout(const BufferLayout& layout) override {}
    virtual void attachFramebuffer(std::shared_ptr<Framebuffer> framebuffer) override {}
	
   private:
    void attachLayout() {}
};
}  // namespace Atlas