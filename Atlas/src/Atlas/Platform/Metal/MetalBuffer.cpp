#include "atpch.h"
#include "MetalBuffer.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    MetalVertexBuffer::MetalVertexBuffer(float* vertices, uint32_t size) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(vertices, size * sizeof(uint32_t), MTL::ResourceStorageModePrivate);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count) : m_indexCount(count) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceStorageModePrivate);
    }

};  // namespace Atlas