#include "MetalVertexArray.h"
#include "atpch.h"

#include "MetalContext.h"

namespace Atlas {

MetalVertexArray::MetalVertexArray() {
    // m_vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
}

MetalVertexArray::~MetalVertexArray() {
    // m_vertexDescriptor->release();
}

// void MetalVertexArray::bind() const {
//     int index = 0;
//     for (auto buffer : m_vertexBuffers) {
//         buffer->bind(index++);
//     }
// }

// void MetalVertexArray::unbind() const {
//     AT_CORE_WARN("MetalVertexArray unbind not implemented");
// }

void MetalVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) {
    // AT_CORE_ASSERT(buffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

    // bind to program how?

    // uint32_t index = 0;
    // const auto& layout = buffer->getLayout();
    // for (const auto& element : layout) {
    //     MTL::VertexAttributeDescriptor* attribute = m_vertexDescriptor->attributes()->object(index);
    //     attribute->setFormat(shaderDataTypeToMTLFormat(element.type));
    //     attribute->setOffset(element.offset);
    //     attribute->setBufferIndex(0);

    //     index++;
    // }
    // MTL::VertexBufferLayoutDescriptor* bufferLayout = m_vertexDescriptor->layouts()->object(0);
    // bufferLayout->setStride(layout.getStride());
    // bufferLayout->setStepFunction(MTL::VertexStepFunctionPerVertex);

    m_vertexBuffers.push_back(buffer);
}

void MetalVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) {
    m_indexBuffer = buffer;
}
}  // namespace Atlas