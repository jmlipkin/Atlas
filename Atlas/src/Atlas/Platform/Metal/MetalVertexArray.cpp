#include "atpch.h"
#include "MetalVertexArray.h"

#include "MetalContext.h"

namespace Atlas {
    MetalVertexArray::MetalVertexArray() : pDevice(MetalContext::getMTLData()->device) {
        pVertexDescriptor = MTL::VertexDescriptor::alloc()->init();
    }

    MetalVertexArray::~MetalVertexArray() {
        pVertexDescriptor->reset();
        pVertexDescriptor->release();
    }

    void MetalVertexArray::bind() const {
        AT_CORE_WARN("MetalVertexArray not implemented");
    }

    void MetalVertexArray::unbind() const {
        AT_CORE_WARN("MetalVertexArray not implemented");
    }

    void MetalVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) {
        AT_CORE_WARN("MetalVertexArray not implemented");
    }

    void MetalVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) {
        AT_CORE_WARN("MetalVertexArray not implemented");
    }

}