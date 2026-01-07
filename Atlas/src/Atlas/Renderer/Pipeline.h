#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "Shader.h"

namespace Atlas {

struct PipelineSpecification {
    std::string name;
    std::shared_ptr<Shader> shader;
    BufferLayout layout;
};

class Pipeline {
   public:
    virtual ~Pipeline() = default;

    virtual void setLayout(const BufferLayout& layout) = 0;

    virtual void setBool(const std::string& name, const bool value) = 0;
    virtual void setInt(const std::string& name, const int value) = 0;
    virtual void setFloat(const std::string& name, const float value) = 0;
    virtual void setFloat2(const std::string& name, const glm::vec2& value) = 0;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
    virtual void setMat3(const std::string& name, const glm::mat3& value) = 0;
    virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

    static std::shared_ptr<Pipeline> create(const PipelineSpecification& specs);
};

}  // namespace Atlas