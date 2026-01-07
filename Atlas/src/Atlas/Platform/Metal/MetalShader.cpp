#include "MetalShader.h"

#include "atpch.h"

#include "MetalContext.h"
#include "MetalRendererAPI.h"

namespace Atlas {

MetalShader::MetalShader(const std::string& name, const std::string& filepath) {
    m_name = name;
    MTL::Library* library = MetalContext::setNewMTLLibrary(filepath);

    m_vertexShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
    m_fragmentShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));
}

MetalShader::MetalShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc) {
    m_name = name;
    m_vertexShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(vertexSrc.c_str(), NS::UTF8StringEncoding));
    m_fragmentShader = MetalContext::getMTLLibrary()->newFunction(NS::String::string(fragSrc.c_str(), NS::UTF8StringEncoding));
}

}  // namespace Atlas