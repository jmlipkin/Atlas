#pragma once

#include <glm/glm.hpp>

namespace Atlas {

class RendererAPI {
   public:
    enum class API {
        None = 0,
        OpenGL = 1,
        Metal = 2
    };

   public:
    virtual void setClearColor(const glm::vec4& color) = 0;
    virtual void clear() = 0;

    inline static API getAPI() { return s_API; }

   private:
    static API s_API;
};

}  // namespace Atlas