#include "MetalUniformBuffer.h"

#include "MetalContext.h"
#include "atpch.h"

namespace Atlas {
MetalUniformBuffer::MetalUniformBuffer(const PipelineSpecification& specs, UniformBufferLayout layout, uint32_t index) : m_layout(layout), m_index(index) {
    AT_PROFILE_FUNCTION();

    MTL::Function* fs = static_cast<MTL::Function*>(specs.shader->getFragmentShader());
    m_argEncoder = fs->newArgumentEncoder(index);
    m_buffer = MetalContext::getMTLDevice()->newBuffer(m_argEncoder->encodedLength(), MTL::ResourceStorageModeShared);
    m_argEncoder->setArgumentBuffer(m_buffer, 0);

    setAlignedOffsetsAndStride();

    AT_CORE_ASSERT(sizesMatch(), "Error: Shader [{0}] uniform layout doesn't match!", specs.shader->getName());
}

void MetalUniformBuffer::setAlignedOffsetsAndStride() {
    AT_PROFILE_FUNCTION();

    uint32_t offset = 0;
    uint32_t stride = 0;

    for (auto& element : m_layout) {
        element.offset = offset;
        offset = offset + element.size + ((m_argEncoder->alignment() - (offset + element.size)) % m_argEncoder->alignment());
        stride = offset;
    }
    m_layout.setStride(stride);
}

bool MetalUniformBuffer::sizesMatch() {
    AT_PROFILE_FUNCTION();

    NS::UInteger GPUStructSize = m_argEncoder->encodedLength();

    if (m_layout.getStride() != GPUStructSize) {
        return false;
    }

    uint32_t index = 0;
    for (auto& element : m_layout) {
        // Must have sequential indices starting from 0
        if(element.id != index) {
            return false;
        }

        size_t offsetCPU = element.offset;
        size_t offsetGPU = static_cast<size_t>(reinterpret_cast<uint8_t*>(m_argEncoder->constantData(index)) - reinterpret_cast<uint8_t*>(m_buffer->contents()));

        if (offsetCPU != offsetGPU) {
            return false;
        }
        index++;
    }

    return true;
}

template <typename T>
void MetalUniformBuffer::updateValue(UniformElement& e, T& value) {
    AT_PROFILE_FUNCTION();
    
    e.value = value;
    memcpy(m_argEncoder->constantData(e.id), &e.value, e.size);
}

void MetalUniformBuffer::setBool(const std::string& name, const bool value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Bool) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setUint(const std::string& name, const int value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Uint) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setInt(const std::string& name, const int value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Int) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setInt2(const std::string& name, const glm::ivec2& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Int2) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setInt3(const std::string& name, const glm::ivec3& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Int3) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setInt4(const std::string& name, const glm::ivec4& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Float) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setFloat(const std::string& name, const float value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Float) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setFloat2(const std::string& name, const glm::vec2& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Float2) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setFloat3(const std::string& name, const glm::vec3& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Float3) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setFloat4(const std::string& name, const glm::vec4& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Float4) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setMat3(const std::string& name, const glm::mat3& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Mat3) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

void MetalUniformBuffer::setMat4(const std::string& name, const glm::mat4& value) {
    UniformElement e = m_layout.getElement(name);
    if (e.type != ShaderDataType::Mat4) {
        AT_CORE_WARN("Could not update uniform value {0}: Type mismatch", name);
        return;
    }
    updateValue(e, value);
}

}  // namespace Atlas