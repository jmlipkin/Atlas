#pragma once

#include "Atlas/Renderer/Pipeline.h"
#include "Atlas/Renderer/Buffer.h"

namespace Atlas {
class OpenGLPipeline : public Pipeline {
   public:
    OpenGLPipeline(const PipelineSpecification& specs) {}

    virtual void setLayout(const BufferLayout& layout) override {}

   private:
    void attachLayout() {}
};
}  // namespace Atlas