#pragma once

#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/Pipeline.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalUniformBuffer : public UniformBuffer {
   public:
    MetalUniformBuffer(const PipelineSpecification& specs, UniformBufferLayout layout, uint32_t index);

    virtual uint32_t getIndex() const override { return m_index; }
    virtual void* getNativeBuffer() const override { return m_buffer; }

    virtual void setBool(const std::string& name, const bool value) override;
    virtual void setInt(const std::string& name, const int value) override;
    virtual void setInt2(const std::string& name, const glm::ivec2& value) override;
    virtual void setInt3(const std::string& name, const glm::ivec3& value) override;
    virtual void setInt4(const std::string& name, const glm::ivec4& value) override;
    virtual void setFloat(const std::string& name, const float value) override;
    virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void setMat3(const std::string& name, const glm::mat3& value) override;
    virtual void setMat4(const std::string& name, const glm::mat4& value) override;

   private:
    void setAlignedOffsetsAndStride();
    bool sizesMatch();

    template <typename T>
    void updateValue(UniformElement& e, T& value);

   private:
    MTL::Buffer* m_buffer;
    MTL::ArgumentEncoder* m_argEncoder;
    UniformBufferLayout m_layout;
    uint32_t m_index;
};
}  // namespace Atlas