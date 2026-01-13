#pragma once

#include <Atlas.h>

#include <glm/glm.hpp>

class SandboxScene : public Atlas::Scene {
   public:
    SandboxScene() : m_cameraController(1280.0/720.0) {
        setRegistryCallbacks();

        m_board = createEntity("Board");

        Atlas::TextureSheetSpecification specs{glm::vec2(8.0f)};
        m_textureSheet = new Atlas::TextureSheet("examples/PacMan/src/pacman_all.png", specs);
        m_board.addComponent<Atlas::Component::Sprite>(m_textureSheet->addSubTexture("board", glm::ivec2(0), m_textureSheet->getSizeInTiles()));
    }
    ~SandboxScene() {
        delete m_textureSheet;
    };

    virtual void onUpdate(Atlas::DeltaTime dt) override {
        Atlas::Renderer::beginScene(m_cameraController.getCamera());
        auto group = m_registry.group<Atlas::Component::Transform, Atlas::Component::Sprite>();
        for(auto entity : group) {
            Atlas::Component::Transform transform = group.get<Atlas::Component::Transform>(entity);
            Atlas::Component::Sprite sprite = group.get<Atlas::Component::Sprite>(entity);

            Atlas::Renderer::drawQuad(transform.position, transform.size, sprite.subtexture);
        }
        Atlas::Renderer::endScene();
    }

    virtual void setRegistryCallbacks() override {
        m_registry.on_construct<Atlas::Component::Sprite>().connect<&Atlas::System::SpriteInitializer::OnSpriteAttach>();
    }

   private:
    Atlas::Entity m_board;

    Atlas::OrthographicCameraController m_cameraController;
    Atlas::TextureSheet* m_textureSheet;
};