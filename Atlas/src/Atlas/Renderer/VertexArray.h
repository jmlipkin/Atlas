#pragma once

#include <memory>
#include <vector>
#include "Atlas/Renderer/Buffer.h"

namespace Atlas {

    class VertexArray {
        public:
         virtual ~VertexArray() = default;

         virtual void bind() const = 0;
         virtual void unbind() const = 0;

         virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) = 0;
         virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) = 0;

         virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;
         virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;

         static VertexArray* create();
    };

}  // namespace Atlas