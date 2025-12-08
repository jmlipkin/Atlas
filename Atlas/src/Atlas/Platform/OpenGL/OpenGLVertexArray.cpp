#include "OpenGLVertexArray.h"

#include "Atlas/Renderer/VertexArray.h"
#include "atpch.h"

#include <glad/glad.h>

namespace Atlas {

    static GLenum shaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
            case Atlas::ShaderDataType::None:     return GL_NONE;
			case Atlas::ShaderDataType::Float:    return GL_FLOAT;
			case Atlas::ShaderDataType::Float2:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float3:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float4:   return GL_FLOAT;
			case Atlas::ShaderDataType::Mat3:     return GL_FLOAT;
			case Atlas::ShaderDataType::Mat4:     return GL_FLOAT;
			case Atlas::ShaderDataType::Int:      return GL_INT;
			case Atlas::ShaderDataType::Int2:     return GL_INT;
			case Atlas::ShaderDataType::Int3:     return GL_INT;
			case Atlas::ShaderDataType::Int4:     return GL_INT;
			case Atlas::ShaderDataType::Bool:     return GL_BOOL;
		}
		
		AT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

    OpenGLVertexArray::OpenGLVertexArray() {
        glGenVertexArrays(1, &m_rendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_rendererID);
    }

    void OpenGLVertexArray::bind() const {
        glBindVertexArray(m_rendererID);
    }

    void OpenGLVertexArray::unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {

        AT_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_rendererID);
		vertexBuffer->bind();

        uint32_t index = 0;
        const auto& layout = vertexBuffer->getLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                                element.GetComponentCount(),
                                shaderDataTypeToOpenGLBaseType(element.type),
                                element.normalized ? GL_TRUE : GL_FALSE,
                                layout.getStride(),
                                (const void*)element.offset);
            index++;
        }

        m_vertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) {
        glBindVertexArray(m_rendererID);
        buffer->bind();
        m_indexBuffer = buffer;
    }

}  // namespace Atlas