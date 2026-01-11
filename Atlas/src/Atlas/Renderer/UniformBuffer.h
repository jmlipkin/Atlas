#pragma once

#include <glm/glm.hpp>

#include "Atlas/Renderer/Pipeline.h"

namespace Atlas {

using UniformValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, int, glm::ivec2, glm::ivec3, glm::ivec4, bool>;
struct UniformElement {
    std::string name;
    uint32_t id;
    UniformValue value;
    ShaderDataType type;
    size_t size;
    size_t offset;

    template <typename T>
    UniformElement(const std::string& u_name, uint32_t u_id, const T& u_value) : name(u_name), id(u_id), value(u_value) {
        if (std::is_same_v<T, float>) {
            type = ShaderDataType::Float;
            size = sizeof(float);
        } else if (std::is_same_v<T, glm::vec2>) {
            type = ShaderDataType::Float2;
            size = sizeof(glm::vec2);
        } else if (std::is_same_v<T, glm::vec3>) {
            type = ShaderDataType::Float3;
            size = sizeof(glm::vec3);
        } else if (std::is_same_v<T, glm::vec4>) {
            type = ShaderDataType::Float4;
            size = sizeof(glm::vec4);
        } else if (std::is_same_v<T, glm::mat3>) {
            type = ShaderDataType::Mat3;
            size = sizeof(glm::mat3);
        } else if (std::is_same_v<T, glm::mat4>) {
            type = ShaderDataType::Mat4;
            size = sizeof(glm::mat4);
        } else if (std::is_same_v<T, uint32_t>) {
            type = ShaderDataType::Uint;
            size = sizeof(uint32_t);
        } else if (std::is_same_v<T, int>) {
            type = ShaderDataType::Int;
            size = sizeof(int);
        } else if (std::is_same_v<T, glm::ivec2>) {
            type = ShaderDataType::Int2;
            size = sizeof(glm::ivec2);
        } else if (std::is_same_v<T, glm::ivec3>) {
            type = ShaderDataType::Int3;
            size = sizeof(glm::ivec3);
        } else if (std::is_same_v<T, glm::ivec4>) {
            type = ShaderDataType::Int4;
            size = sizeof(glm::ivec4);
        } else if (std::is_same_v<T, bool>) {
            type = ShaderDataType::Bool;
            size = sizeof(bool);
        } else {
            type = ShaderDataType::None;
            size = 0;
            AT_CORE_ERROR("Shader data type is not supported!");
        }
        offset = 0;
    }
};

class UniformBufferLayout {
   public:
    UniformBufferLayout() {}
    UniformBufferLayout(const std::initializer_list<UniformElement>& elements) : m_elements(elements) {
        addToMap();
        calculateOffsetsAndStride();
    }

    UniformElement& getElement(const std::string& name);
    UniformElement& getElement(const uint32_t id);

    inline void setStride(uint32_t stride) { m_stride = stride; }
    inline uint32_t getStride() const { return m_stride; }
    inline const std::vector<UniformElement>& getElements() const { return m_elements; }

    UniformElement& operator[](size_t index) { return m_elements[index]; }
    std::vector<UniformElement>::iterator begin() { return m_elements.begin(); }
    std::vector<UniformElement>::iterator end() { return m_elements.end(); }
    std::vector<UniformElement>::const_iterator begin() const { return m_elements.begin(); }
    std::vector<UniformElement>::const_iterator end() const { return m_elements.end(); }

   private:
    void addToMap() {
        for (auto& element : m_elements) {
            m_map[element.name] = element.id;
        }
    }

    void calculateOffsetsAndStride() {
        uint32_t offset = 0;
        m_stride = 0;
        for (auto& element : m_elements) {
            element.offset = offset;
            offset += element.size;
            m_stride += element.size;
        }
    }

   private:
    std::vector<UniformElement> m_elements;
    std::unordered_map<std::string, uint32_t> m_map;
    uint32_t m_stride = 0;
};

class UniformBuffer {
   public:
    virtual ~UniformBuffer() = default;

    virtual uint32_t getIndex() const = 0;
    virtual void* getNativeBuffer() const = 0;

    virtual void setBool(const std::string& name, const bool value) = 0;
    virtual void setUint(const std::string& name, const int value) = 0;
    virtual void setInt(const std::string& name, const int value) = 0;
    virtual void setInt2(const std::string& name, const glm::ivec2& value) = 0;
    virtual void setInt3(const std::string& name, const glm::ivec3& value) = 0;
    virtual void setInt4(const std::string& name, const glm::ivec4& value) = 0;
    virtual void setFloat(const std::string& name, const float value) = 0;
    virtual void setFloat2(const std::string& name, const glm::vec2& value) = 0;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
    virtual void setMat3(const std::string& name, const glm::mat3& value) = 0;
    virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

    static std::shared_ptr<UniformBuffer> create(const PipelineSpecification& specs, const UniformBufferLayout& layout, uint32_t index);
};

}  // namespace Atlas