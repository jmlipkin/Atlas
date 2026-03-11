#pragma once

#include <string>

namespace Atlas {
class Shader {
   public:
    virtual ~Shader() = default;

    virtual void* getVertexShader() const = 0;
    virtual void* getFragmentShader() const = 0;

    virtual const std::string& getName() const = 0;

    static std::shared_ptr<Shader> create(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction);
};

}  // namespace Atlas