#pragma once

#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {
class OpenGLPipeline : public Pipeline {
   public:
    OpenGLPipeline(const PipelineSpecification& specs) {}

    virtual void setLayout(const BufferLayout& layout) override {}
    virtual std::shared_ptr<PipelineReflection> getReflection() const override { return m_reflection; }

    // virtual void setBool(const std::string& name, const bool value) override {}
    // virtual void setInt(const std::string& name, const int value) override {}
    // virtual void setFloat(const std::string& name, const float value) override {}
    // virtual void setFloat2(const std::string& name, const glm::vec2& value) override {}
    // virtual void setFloat3(const std::string& name, const glm::vec3& value) override {}
    // virtual void setFloat4(const std::string& name, const glm::vec4& value) override {}
    // virtual void setMat3(const std::string& name, const glm::mat3& value) override {}
    // virtual void setMat4(const std::string& name, const glm::mat4& value) override {}

   private:
    void attachLayout() {}

    private:
     std::shared_ptr<PipelineReflection> m_reflection;
};
}  // namespace Atlas