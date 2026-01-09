#pragma once

#include "Atlas/Renderer/Buffer.h"

namespace Atlas {

    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(uint32_t size);
        OpenGLVertexBuffer(float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        void* getNativeBuffer() const override { return (void*)&m_rendererID; }

       private:
        uint32_t m_rendererID;
    };

    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual uint32_t getCount() const override { return m_indexCount; }

    private:
        uint32_t m_rendererID;
        uint32_t m_indexCount;
    };

}  // namespace Atlas