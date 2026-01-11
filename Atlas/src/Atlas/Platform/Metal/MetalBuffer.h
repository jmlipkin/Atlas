#pragma once

#include "Atlas/Renderer/Buffer.h"

#include "MetalContext.h"

namespace Atlas {

    class MetalVertexBuffer : public VertexBuffer {
    public:
     MetalVertexBuffer(uint32_t size);
     MetalVertexBuffer(float* vertices, uint32_t size);
     virtual ~MetalVertexBuffer() = default;

     virtual void setData(const void* data, uint32_t size) override;

     void* getNativeBuffer() const override { return m_buffer; }

    private:
     MTL::Buffer* m_buffer;
    };

    class MetalIndexBuffer : public IndexBuffer {
    public:
        MetalIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~MetalIndexBuffer() = default;

        virtual uint32_t getCount() const override { return m_indexCount; }

        virtual MTL::Buffer* getBuffer() const { return m_buffer; }

    private:
     MTL::Buffer* m_buffer;
     uint32_t m_indexCount;
    };

}  // namespace Atlas