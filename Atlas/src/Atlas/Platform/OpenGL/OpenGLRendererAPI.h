#pragma once

#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Platform/OpenGL/OpenGLBuffer.h"

namespace Atlas {

    class OpenGLRendererAPI : public RendererAPI {
       public:
        virtual ~OpenGLRendererAPI() = default;
        virtual void setClearColor(const glm::vec4& color) override;
        virtual void clear() override;
        virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
        
        // TODO: Implement
        virtual void beginImGui() override {}
        virtual void drawImGui() override {}
        void bindPipeline(const Pipeline& pipeline) override {}
        void bindVertexArray(const VertexArray& array) override {}
        void bindVertexBuffer(const VertexBuffer& buffer, int index) override {}
        virtual void beginFrame() override;
        virtual void endFrame() override {}
        virtual void commit() override {}

        // TEMPORARY?
        virtual void setBool(const std::string& name, const bool value) override {}
        virtual void setInt(const std::string& name, const int value) override {}
        virtual void setFloat(const std::string& name, const float value) override {}
        virtual void setFloat2(const std::string& name, const glm::vec2& value) override {}
        virtual void setFloat3(const std::string& name, const glm::vec3& value) override {}
        virtual void setFloat4(const std::string& name, const glm::vec4& value) override {}
        virtual void setMat3(const std::string& name, const glm::mat3& value) override {}
        virtual void setMat4(const std::string& name, const glm::mat4& value) override {}
    };

}  // namespace Atlas