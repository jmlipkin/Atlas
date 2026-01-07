#pragma once

#include "Atlas/Renderer/VertexArray.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

    class MetalVertexArray : public VertexArray {
        public:
         MetalVertexArray();
         virtual ~MetalVertexArray() = default;

         virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) override;
         virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) override;

         virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; }
         virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; }

        private:
         std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
         std::shared_ptr<IndexBuffer> m_indexBuffer;
    };

}  // namespace Atlas