#include "atpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Atlas {
        void OpenGLRendererAPI::setClearColor(const glm::vec4& color) {
            glClearColor(color.r, color.g, color.b, color.a);
        }

        void OpenGLRendererAPI::clear() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void OpenGLRendererAPI::beginFrame(std::shared_ptr<Framebuffer> framebuffer) {
            clear();
        }
    }  // namespace Atlas