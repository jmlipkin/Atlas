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

        void OpenGLRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
            glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
        }

        void OpenGLRendererAPI::onEvent(Event& event) {
            EventDispatcher dispatcher(event);
            dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(OpenGLRendererAPI::onWindowResize));
        }

        bool OpenGLRendererAPI::onWindowResize(WindowResizeEvent& e) {
            glViewport(0, 0, e.getWidth(), e.getHeight());

            return false;
        }
    }  // namespace Atlas