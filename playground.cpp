#include <metal-cpp/Metal.hpp>
// per pipeline
MTL::ArgumentEncoder* argEnc = fs->newArgumentEncoder(1);
MTL::Buffer* uBuf = MetalContext::getMTLDevice()->newBuffer(argEnc->encodedLength(), MTL::ResourceStorageModeShared);
argEnc->setArgumentBuffer(uBuf, 0);
NS::UInteger length = argEnc->encodedLength();


// color data
glm::vec4 c{0.8f, 0.1f, 0.1f, 1.0f};

// per update
memcpy(argEnc->constantData(0), &c, sizeof(c));        // constantData( n ) == shader [[id( n )]] ---- c == data to upload

// per draw
m_encoder->setFragmentBuffer(p.getFragBuf(), 0, 1);