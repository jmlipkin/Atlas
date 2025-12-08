#include "atpch.h"
#include "MetalBuffer.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    MetalVertexBuffer::MetalVertexBuffer(float* vertices, uint32_t size) {
        m_device = MetalContext::getMTLDevice();
        m_buffer = m_device->newBuffer(vertices, size * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count) : m_indexCount(count) {
        m_device = MetalContext::getMTLDevice();
        m_buffer = m_device->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

};  // namespace Atlas