#include "atpch.h"
#include "MetalRendererAPI.h"

#include <metal-cpp/Metal.hpp>

namespace Atlas {

    void setClearColor(const glm::vec4& color) {
        // MTL::ClearColor::Make((double)color.r, (double)color.g, (double)color.b, (double)color.a);
        AT_CORE_WARN("MetalRendererAPI setClearColor(const glm::vec4& color) not implemented");
    }
    
    void clear() {
        AT_CORE_WARN("MetalRendererAPI clear() not implemented");
    }

}