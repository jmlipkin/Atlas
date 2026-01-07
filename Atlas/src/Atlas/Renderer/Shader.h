#pragma once

#include <string>

namespace Atlas {
class Shader {
   public:
    virtual ~Shader() = default;

    virtual void* getVertexShader() const = 0;
    virtual void* getFragmentShader() const = 0;

    virtual const std::string& getName() const = 0;

    static std::shared_ptr<Shader> create(const std::string& filepath);
    static std::shared_ptr<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
};

class ShaderLibrary {
    public:
     void add(std::string name, const std::shared_ptr<Shader>& shader);
     void add(const std::shared_ptr<Shader>& shader);

     std::shared_ptr<Shader> load(const std::string& filepath);
     std::shared_ptr<Shader> load(const std::string& name, const std::string& filepath);

     std::shared_ptr<Shader> get(const std::string& name) const;

     bool exists(const std::string& name) const;

    private:
     std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};
}  // namespace Atlas