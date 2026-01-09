#include "atpch.h"
#include "MetalBuffer.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    MetalVertexBuffer::MetalVertexBuffer(uint32_t size) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(size * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    MetalVertexBuffer::MetalVertexBuffer(float* vertices, uint32_t size) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(vertices, size * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count) : m_indexCount(count) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

};  // namespace Atlas