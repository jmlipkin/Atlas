#pragma once

#include "Atlas/Renderer/Buffer.h"

namespace Atlas {

    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();
        virtual void bind() override;
        virtual void unbind() override;

    private:
        uint32_t m_rendererID;
    };
    
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();
        virtual void bind() override;
        virtual void unbind() override;
        virtual uint32_t getCount() override { return m_indexCount; }

    private:
        uint32_t m_rendererID;
        uint32_t m_indexCount;
    };

}  // namespace Atlas