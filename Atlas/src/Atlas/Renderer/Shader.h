#pragma once

#include <string>

namespace Atlas {
    class Shader {
        public:
         Shader(const std::string& vertexSrc, const std::string& fragSrc);
         ~Shader();

         void bind() const;
         void unbind() const;
         private:
          uint32_t m_rendererID;
    };
}  // namespace Atlas