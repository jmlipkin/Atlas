#include "atpch.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {
    MetalRendererAPI::MetalRendererAPI() : pDevice(*MetalContext::getMTLDevice()) {
        AT_CORE_INFO("MTLDevice name: {} (from MetalRendererAPI)", pDevice.name()->cString(NS::ASCIIStringEncoding));
    }

    void MetalRendererAPI::setClearColor(const glm::vec4& color) {
        // MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
        AT_CORE_WARN("MetalRendererAPI setClearColor(const glm::vec4& color) not implemented");
    }
    
    void MetalRendererAPI::clear() {
        AT_CORE_WARN("MetalRendererAPI clear() not implemented");
    }

    void MetalRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
        AT_CORE_WARN("MetalRendererAPI drawIndexed() not implemented");
    }

}