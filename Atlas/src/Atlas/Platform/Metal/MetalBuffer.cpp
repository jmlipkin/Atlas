#include "atpch.h"
#include "MetalBuffer.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    MetalVertexBuffer::MetalVertexBuffer(float* vertices, uint32_t size) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(vertices, size * sizeof(uint32_t), MTL::ResourceStorageModePrivate);
    }

    // void MetalVertexBuffer::bind() const {
    //     // MetalRendererAPI::getEncoder()->setVertexBuffer(m_buffer, 0, 0);
    //     AT_CORE_WARN("MetalVertexBuffer bind not implemented");
    // }

    // void MetalVertexBuffer::bind(int index) const {
    //     // MetalRendererAPI::getEncoder()->setVertexBuffer(m_buffer, 0, index);
    //     AT_CORE_WARN("MetalVertexBuffer bind(int index) not implemented");
    // }

    // void MetalVertexBuffer::unbind() const {}

    MTL::Buffer* MetalVertexBuffer::getNativeBuffer() const {
        return m_buffer;
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count) : m_indexCount(count) {
        m_buffer = MetalContext::getMTLDevice()->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceStorageModePrivate);
    }

};  // namespace Atlas