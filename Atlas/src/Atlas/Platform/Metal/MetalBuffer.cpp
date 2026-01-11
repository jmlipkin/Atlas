#include "atpch.h"
#include "MetalBuffer.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    MetalVertexBuffer::MetalVertexBuffer(uint32_t size) {
        AT_PROFILE_FUNCTION();
        
        m_buffer = MetalContext::getMTLDevice()->newBuffer(size * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    MetalVertexBuffer::MetalVertexBuffer(float* vertices, uint32_t size) {
        AT_PROFILE_FUNCTION();

        m_buffer = MetalContext::getMTLDevice()->newBuffer(vertices, size * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    void MetalVertexBuffer::setData(const void* data, uint32_t size) {
        AT_PROFILE_FUNCTION();

        NS::UInteger existingSize = m_buffer->allocatedSize();
        AT_CORE_ASSERT(size <= existingSize, "VertexBuffer data could not be set: submitted size exceeds allocated space");

        void* dst = m_buffer->contents();
        memcpy(dst, data, size);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count) : m_indexCount(count) {
        AT_PROFILE_FUNCTION();
        
        m_buffer = MetalContext::getMTLDevice()->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

};  // namespace Atlas