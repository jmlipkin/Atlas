#pragma once

#include "Atlas/Renderer/Buffer.h"

#include "MetalContext.h"

namespace Atlas {

    class MetalVertexBuffer : public VertexBuffer {
    public:
        MetalVertexBuffer(float* vertices, uint32_t size);
        virtual ~MetalVertexBuffer() = default;
        virtual void bind() {}
        virtual void unbind() {}

        virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
        virtual const BufferLayout& getLayout() const override { return m_layout; }

       private:
        MTL::Device* m_device;
        MTL::Buffer* m_buffer;
        BufferLayout m_layout;
    };

    class MetalIndexBuffer : public IndexBuffer {
    public:
        MetalIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~MetalIndexBuffer() = default;
        virtual void bind() {}
        virtual void unbind() {}
        virtual uint32_t getCount() const override { return m_indexCount; }

    private:
     MTL::Device* m_device;
     MTL::Buffer* m_buffer;
     uint32_t m_indexCount;
    };

}  // namespace Atlas