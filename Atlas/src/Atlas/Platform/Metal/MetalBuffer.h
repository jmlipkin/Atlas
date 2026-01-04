#pragma once

#include "Atlas/Renderer/Buffer.h"

#include "MetalContext.h"

namespace Atlas {

    class MetalVertexBuffer : public VertexBuffer {
    public:
        MetalVertexBuffer(float* vertices, uint32_t size);
        virtual ~MetalVertexBuffer() = default;
        virtual void bind() const override;
        virtual void bind(int index) const override;
        virtual void unbind() const override;

        virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
        virtual const BufferLayout& getLayout() const override { return m_layout; }

       private:
        MTL::Buffer* m_buffer;
        BufferLayout m_layout;
    };

    class MetalIndexBuffer : public IndexBuffer {
    public:
        MetalIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~MetalIndexBuffer() = default;
        virtual void bind() const override {}
        virtual void unbind() const override {}
        virtual uint32_t getCount() const override { return m_indexCount; }

        virtual MTL::Buffer* getBuffer() const { return m_buffer; }

    private:
     MTL::Buffer* m_buffer;
     uint32_t m_indexCount;
    };

}  // namespace Atlas