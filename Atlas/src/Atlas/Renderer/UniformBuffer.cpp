#include "atpch.h"
#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "Atlas/Platform/Metal/MetalUniformBuffer.h"

namespace Atlas {

    std::shared_ptr<UniformBuffer> UniformBuffer::create(const PipelineSpecification& specs, const UniformBufferLayout& layout, uint32_t index) {
        switch(RendererAPI::getAPI()) {
            case RendererAPI::API::None: {
                AT_ASSERT(false, "RendererAPI::None is not supported");
                break;
            }
            case RendererAPI::API::OpenGL: {
                AT_ASSERT(false, "RendererAPI::OpenGL is not supported (UniformBuffer)");
                break;
            }
            case RendererAPI::API::Metal: {
                return std::make_shared<MetalUniformBuffer>(specs, layout, index);
                break;
            }
        }

        AT_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    UniformElement& UniformBufferLayout::getElement(const std::string& name) {
        return getElement(m_map.at(name));
    }

    UniformElement& UniformBufferLayout::getElement(const uint32_t id) {
        return m_elements[id];
    }
}