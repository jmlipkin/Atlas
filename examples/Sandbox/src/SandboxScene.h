#pragma once

#include <Atlas.h>

#include <glm/glm.hpp>

class SandboxScene : public Atlas::Scene {
   public:
    SandboxScene() : m_cameraController(1280.0 / 720.0) {
        setRegistryCallbacks();

        m_cameraController.setZoomLevel(25.0f);
        m_board = createEntity("Board");
        m_player = createEntity("PacMan");

        Atlas::TextureSheetSpecification specs{glm::vec2(8.0f)};
        m_textureSheet = new Atlas::TextureSheet("examples/PacMan/src/pacman_all.png", specs);
        m_board.addComponent<Atlas::Component::Sprite>(m_textureSheet->addSubTexture("board", glm::ivec2(0), m_textureSheet->getSizeInTiles()));
        m_player.addComponent<Atlas::Component::Sprite>(m_textureSheet->addSubTexture("player", glm::ivec2(0, 0), glm::ivec2(2,2)));

        // position board at center
        Atlas::Component::Transform& t = m_board.getComponent<Atlas::Component::Transform>();
        t.position = glm::vec3(-t.size.x / 2.0f, -t.size.y / 2.0f, 0.0f);

        std::vector<std::shared_ptr<Atlas::SubTexture>> animDying;
        for (int i = 0; i < 11; i++) {
            std::string tag = "Dying" + std::to_string(i);
            std::shared_ptr<Atlas::SubTexture> frame = m_textureSheet->addSubTexture(tag, glm::ivec2(63 + 2*i, 0), glm::ivec2(2, 2));
            animDying.push_back(frame);
        }
        std::shared_ptr<Atlas::AnimationClip> dyingClip = std::make_shared<Atlas::AnimationClip>(animDying);
        Atlas::Component::Animation& animation = m_player.addComponent<Atlas::Component::Animation>(dyingClip);
        animation.playing = true;
        animation.shouldLoop = false;
        animation.animationSpeed = 0.5;
    }
    ~SandboxScene() {
        delete m_textureSheet;
    };

    virtual void onUpdate(Atlas::DeltaTime dt) override {
        Atlas::System::Animation::updateFrames(m_registry, dt);
        Atlas::Renderer::beginScene(m_cameraController.getCamera());
        {
            auto view = m_registry.view<Atlas::Component::Transform, Atlas::Component::Sprite>();
            for (auto entity : view) {
                Atlas::Component::Transform transform = view.get<Atlas::Component::Transform>(entity);
                Atlas::Component::Sprite sprite = view.get<Atlas::Component::Sprite>(entity);

                Atlas::Renderer::drawQuad(transform.position, transform.size, sprite.subtexture);
            }
        }

        Atlas::Renderer::endScene();
    }

    virtual void setRegistryCallbacks() override {
        m_registry.on_construct<Atlas::Component::Sprite>().connect<&Atlas::System::SpriteInitializer::OnSpriteAttach>();
    }

   private:
    Atlas::Entity m_board;
    Atlas::Entity m_player;

    Atlas::OrthographicCameraController m_cameraController;
    Atlas::TextureSheet* m_textureSheet;
};