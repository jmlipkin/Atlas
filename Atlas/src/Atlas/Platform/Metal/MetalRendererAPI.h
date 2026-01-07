#pragma once

#include "Atlas/Renderer/RendererAPI.h"

#include "Atlas/Platform/Metal/MetalContext.h"
#include <metal-cpp/Metal.hpp>

namespace Atlas {

class MetalRendererAPI : public RendererAPI {
   public:
    MetalRendererAPI(MetalContext& context);
    virtual ~MetalRendererAPI();

    virtual void setClearColor(const glm::vec4& color) override;
    virtual void clear() override;

    virtual void beginFrame() override;
    virtual void endFrame() override;

    // ImGui functions implemented in MetalRendererAPI.mm
    virtual void beginImGui() override;
    virtual void drawImGui() override;

    void bindPipeline(const Pipeline& shader) override;
    void bindVertexArray(const VertexArray& array) override;
    void bindVertexBuffer(const VertexBuffer& buffer, int index) override;

    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    virtual void commit() override;

    // TEMPORARY?
    virtual void setBool(const std::string& name, const bool value) override;
    virtual void setInt(const std::string& name, const int value) override;
    virtual void setFloat(const std::string& name, const float value) override;
    virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void setMat3(const std::string& name, const glm::mat3& value) override;
    virtual void setMat4(const std::string& name, const glm::mat4& value) override;

   private:
    MetalContext& m_context;
    MTL::ClearColor m_color;

    NS::AutoreleasePool* m_pool;
    CA::MetalDrawable* m_drawable;
    MTL::RenderPassDescriptor* m_passDesc;
    MTL::CommandBuffer* m_buffer;

    MTL::CommandQueue* m_commandQueue;
    MTL::RenderCommandEncoder* m_encoder;
};

}  // namespace Atlas