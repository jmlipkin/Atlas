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

class ShaderLibrary {
   public:
    virtual void add(std::string name, const std::shared_ptr<Shader>& shader) = 0;
    virtual void add(const std::shared_ptr<Shader>& shader) = 0;

    virtual std::shared_ptr<Shader> load(const std::string& name, const std::string& vertexFunction, const std::string& fragmentFunction) = 0;
    virtual std::shared_ptr<Shader> get(const std::string& name) const = 0;

    virtual bool exists(const std::string& name) const = 0;

    static std::shared_ptr<ShaderLibrary> create(const std::string& filepath);
};
}  // namespace Atlas