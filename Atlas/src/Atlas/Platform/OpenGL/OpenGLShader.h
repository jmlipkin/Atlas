#pragma once

#include "Atlas/Renderer/Shader.h"
#include <glad/glad.h>

namespace Atlas {

class OpenGLShader : public Shader {
   public:
    OpenGLShader(const std::string& name, const std::string& filepath);
    OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
    virtual ~OpenGLShader();

    // TODO: Implement
    virtual void* getVertexShader() const override { return nullptr; }
    virtual void* getFragmentShader() const override { return nullptr; }

    virtual const std::string& getName() const override { return m_name; }

   private:
    void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

   private:
    uint32_t m_rendererID;
    std::string m_name;
};

}  // namespace Atlas