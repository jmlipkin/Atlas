#include "OpenGLShader.h"

#include <glad/glad.h>

#include "atpch.h"

namespace Atlas {

OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath) {
    AT_CORE_ERROR("Cannot read shaders from file using OpenGL");
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc) : m_name(name) {
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragSrc;
    compile(sources);
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_rendererID);
}

// void OpenGLShader::bind() const {
//     glUseProgram(m_rendererID);
// }
// void OpenGLShader::unbind() const {
//     glUseProgram(0);
// }

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* source = shaderSources.at(GL_VERTEX_SHADER).c_str();
    glShaderSource(vShader, 1, &source, 0);

    glCompileShader(vShader);

    GLint isCompiled = 0;

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(vShader);

        AT_CORE_ERROR("{0}", infoLog.data());
        AT_CORE_ASSERT(false, "Vertex shader compilation failure!");
        return;
    }

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    source = shaderSources.at(GL_FRAGMENT_SHADER).c_str();
    glShaderSource(fShader, 1, &source, 0);

    glCompileShader(fShader);

    isCompiled = 0;

    glGetShaderiv(fShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(fShader);

        AT_CORE_ERROR("{0}", infoLog.data());
        AT_CORE_ASSERT(false, "Fragment shader compilation failure!");
        return;
    }

    m_rendererID = glCreateProgram();
    GLuint program = m_rendererID;

    glAttachShader(program, vShader);
    glAttachShader(program, fShader);

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        AT_CORE_ERROR("{0}", infoLog.data());
        AT_CORE_ASSERT(false, "Shader link failure!");
        return;
    }

    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
}
}  // namespace Atlas