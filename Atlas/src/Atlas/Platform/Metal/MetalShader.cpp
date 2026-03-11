#include "MetalShader.h"

#include "atpch.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

namespace Atlas {

MetalShader::MetalShader(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) {
    AT_PROFILE_FUNCTION();

    m_name = name;
    m_vertexShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(vertexFunction.c_str(), NS::UTF8StringEncoding));
    m_fragmentShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(fragmentFunction.c_str(), NS::UTF8StringEncoding));
}

}  // namespace Atlas