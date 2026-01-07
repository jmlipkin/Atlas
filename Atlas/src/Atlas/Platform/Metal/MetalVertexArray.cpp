#include "MetalVertexArray.h"
#include "atpch.h"

#include "MetalContext.h"

namespace Atlas {

MetalVertexArray::MetalVertexArray() {}

void MetalVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) {
    m_vertexBuffers.push_back(buffer);
}

void MetalVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) {
    m_indexBuffer = buffer;
}
}  // namespace Atlas