#include "atpch.h"
#include "MetalBuffer.h"

#include "MetalContext.h"

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

void MetalIndexBuffer::setData(const void* data, uint32_t count) {
	AT_PROFILE_FUNCTION();

	m_indexCount = count;

	NS::UInteger existingSize = m_buffer->allocatedSize();
	AT_CORE_ASSERT(count * sizeof(uint32_t) <= existingSize, "IndexBuffer data could not be set: submitted size exceeds allocated space");

	void* dst = m_buffer->contents();
	memcpy(dst, data, count * sizeof(uint32_t));
}

};	// namespace Atlas