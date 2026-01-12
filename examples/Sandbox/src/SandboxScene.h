#pragma once

#include <Atlas.h>

class SandboxScene : public Atlas::Scene {
   public:
    SandboxScene() : m_cameraController(1280.0/720.0) {
        m_rectangle = createEntity("Rectangle");
        m_rectangle.addComponent<Atlas::Component::Transform>();
    }
    ~SandboxScene() = default;

    virtual void onUpdate(Atlas::DeltaTime dt) override {
        Atlas::Renderer::beginScene(m_cameraController.getCamera());
        auto view = m_registry.view<Atlas::Component::Transform>();
        for(auto entity : view) {
            Atlas::Component::Transform transform = view.get<Atlas::Component::Transform>(entity);

            Atlas::Renderer::drawQuad(transform.position, transform.size, glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});
        }
        Atlas::Renderer::endScene();
    }

   private:
    Atlas::Entity m_rectangle;

    Atlas::OrthographicCameraController m_cameraController;
};