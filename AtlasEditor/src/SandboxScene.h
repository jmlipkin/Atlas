#pragma once

#include <Atlas.h>

#include <glm/glm.hpp>
#include "Atlas/Core/AssetManager.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Renderer/OrthographicCameraController.h"

#define BOARD_DEPTH	 0.1f
#define PLAYER_DEPTH 100

class PlayerBehavior : public Atlas::Behavior {
  public:
	virtual void onUpdate(Atlas::DeltaTime dt) {
		using namespace Atlas;
		auto& transform = getComponent<Component::Transform>();
		if (Input::isKeyPressed(AT_KEY_W)) {
			transform.position.y -= m_playerSpeed * dt;
		}
		if (Input::isKeyPressed(AT_KEY_S)) {
			transform.position.y += m_playerSpeed * dt;
		}
		if (!Input::isKeyPressed(AT_KEY_W) && !Input::isKeyPressed(AT_KEY_S)) {
			if (Input::isKeyPressed(AT_KEY_A)) {
				transform.position.x -= m_playerSpeed * dt;
			}
			if (Input::isKeyPressed(AT_KEY_D)) {
				transform.position.x += m_playerSpeed * dt;
			}
		}
	}

  private:
	float m_playerSpeed = 10;
};

class SandboxScene : public Atlas::Scene {
  public:
	SandboxScene(Atlas::OrthographicCameraController& cameraController) : m_cameraController(cameraController) {
		setRegistryCallbacks();

		// m_font = Atlas::FontLibrary::load("Helvetica Light", "Atlas/assets/helvetica-neue-5/HelveticaNeueLight.otf");
		// m_font = Atlas::FontLibrary::load("Jetsy", "Atlas/assets/jetsy/Jetsy Trial.otf");
		// m_font = Atlas::FontLibrary::load("Starlight Romance", "Atlas/assets/starlight_romance/Starlight Romance.ttf");
		// m_font = Atlas::FontLibrary::load("CrackMan", "examples/PacMan/assets/fonts/CrackMan.ttf");
		m_font = Atlas::AssetManager::loadFont("Emulogic", "examples/PacMan/assets/fonts/emulogic.ttf");

		m_cameraController.setZoomLevel(25.0f);
		m_board	 = createEntity("Board");
		m_player = createEntity("PacMan");

		Atlas::TextureSheetSpecification specs{glm::vec2(8.0f)};
		m_textureSheet = new Atlas::TextureSheet("examples/PacMan/assets/pacman sprite sheet transparent.png", specs);
		m_board.addComponent<Atlas::Component::Sprite>(m_textureSheet->addSubTexture("board", glm::ivec2(0), glm::ivec2(28, 31)));
		m_player.addComponent<Atlas::Component::Sprite>(m_textureSheet->addSubTexture("player", glm::ivec2(28, 0), glm::ivec2(2, 2)));

		// position board at center
		Atlas::Component::Transform& t		 = m_board.getComponent<Atlas::Component::Transform>();
		t.position							 = glm::vec3(-t.size.x / 2.0f, -t.size.y / 2.0f, BOARD_DEPTH);
		Atlas::Component::Transform& tPlayer = m_player.getComponent<Atlas::Component::Transform>();
		tPlayer.position					 = glm::vec3(-tPlayer.size.x / 2.0f, -tPlayer.size.y / 2.0f, PLAYER_DEPTH);

		std::vector<std::shared_ptr<Atlas::SubTexture>> animDying;
		for (int i = 0; i < 11; i++) {
			std::string						   tag	 = "Dying" + std::to_string(i);
			std::shared_ptr<Atlas::SubTexture> frame = m_textureSheet->addSubTexture(tag, glm::ivec2(34 + 2 * i, 0), glm::ivec2(2, 2));
			animDying.push_back(frame);
		}
		std::shared_ptr<Atlas::AnimationClip> dyingClip = std::make_shared<Atlas::AnimationClip>(animDying);
		Atlas::Component::Animation&		  animation = m_player.addComponent<Atlas::Component::Animation>(dyingClip);
		animation.playing								= true;
		animation.shouldLoop							= true;
		animation.animationSpeed						= 0.5;

		m_player.addScript<PlayerBehavior>();
	}

	~SandboxScene() {
		delete m_textureSheet;
	};

	virtual void onUpdate(Atlas::DeltaTime dt) override {
		{
			auto view = m_registry.view<Atlas::Component::Script>();
			for (auto entity : view) {
				Atlas::Component::Script& script = view.get<Atlas::Component::Script>(entity);
				script.instance->onUpdate(dt);
			}
		}

		Atlas::System::Animation::updateFrames(m_registry, dt);

		Atlas::Renderer::beginScene(m_cameraController.getCamera());
		{
			auto view = m_registry.view<Atlas::Component::Transform, Atlas::Component::Sprite>();
			for (auto entity : view) {
				Atlas::Component::Transform transform = view.get<Atlas::Component::Transform>(entity);
				Atlas::Component::Sprite	sprite	  = view.get<Atlas::Component::Sprite>(entity);

				Atlas::Renderer::drawQuad(transform.position, transform.size, sprite.subtexture);
			}

			Atlas::Renderer::drawText(m_font, "PacMan", glm::vec3(-15, -18, 100), 72);
			Atlas::Renderer::drawText(m_font, "High Score: ", glm::vec3(22, -14, 99.0), 16);
		}
		{
			auto view = m_registry.view<Atlas::Component::Sprite, Atlas::Component::Animation>();
			for (auto entity : view) {
				Atlas::Component::Animation a = view.get<Atlas::Component::Animation>(entity);
				if (!a.playing) {
					Atlas::Renderer::drawText(m_font, "Game Over", glm::vec3(-5, 2.5, 99.0), 16, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				}
			}
		}

		Atlas::Renderer::endScene();
	}

	virtual void onEvent(Atlas::Event& event) override {
		m_cameraController.onEvent(event);
	}

	virtual void setRegistryCallbacks() override {
		m_registry.on_construct<Atlas::Component::Sprite>().connect<&Atlas::System::SpriteInitializer::OnSpriteAttach>();
	}

  private:
	Atlas::Entity m_board;
	Atlas::Entity m_player;

	Atlas::OrthographicCameraController& m_cameraController;
	Atlas::TextureSheet*				 m_textureSheet;

	std::shared_ptr<Atlas::Font> m_font;
};