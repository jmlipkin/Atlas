#pragma once

#include "Atlas/Core/Time.h"

#include "Atlas/ECS/Registry.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"

namespace Atlas::System {

struct Transformation {
	static glm::vec3 getCenter(Entity& entity) {
		AT_CORE_ASSERT(entity.hasComponent<Component::Transform>() && entity.hasComponent<Component::Sprite>(), "Entity must have a transform and sprite component in order to get center");

		Component::Transform t		= entity.getComponent<Component::Transform>();
		Component::Sprite	 sprite = entity.getComponent<Component::Sprite>();
		return {t.position.x + (0.5f * t.size.x), t.position.y + (0.5f * t.size.y), t.position.z};
	}
	static glm::vec2 getCenter2D(Entity& entity) {
		AT_CORE_ASSERT(entity.hasComponent<Component::Transform>() && entity.hasComponent<Component::Sprite>(), "Entity must have a transform and sprite component in order to get center");

		Component::Transform t		= entity.getComponent<Component::Transform>();
		Component::Sprite	 sprite = entity.getComponent<Component::Sprite>();
		return {t.position.x + (0.5f * t.size.x), t.position.y + (0.5f * t.size.y)};
	}

	static void setCenter(Entity& entity, glm::vec2 position) {
		if (!entity.hasComponent<Component::Sprite>() || !entity.hasComponent<Component::Transform>()) {
			return;
		}

		Component::Transform& transform = entity.getComponent<Component::Transform>();
		Component::Sprite&	  sprite	= entity.getComponent<Component::Sprite>();

		glm::vec2 topleft  = position - (0.5f * transform.size);
		transform.position = glm::vec3(topleft, transform.position.z);
	}
};

struct Animation {
	static void updateFrames(Registry& registry, Atlas::DeltaTime dt) {
		auto view = registry.view<Component::Sprite, Component::Animations>();
		for (auto entity : view) {
			Component::Sprite&	   sprite	 = view.get<Component::Sprite>(entity);
			Component::Animations& animation = view.get<Component::Animations>(entity);

			if (!animation.containsActiveClip())
				continue;

			AnimationClip& clip = animation.clips[animation.activeClip];

			if (!clip.playing)
				continue;

			clip.timeSinceLastFrame += dt;
			if (clip.timeSinceLastFrame >= 1.0f / (clip.frameRate)) {
				clip.currentFrame++;
				clip.timeSinceLastFrame = 0.0f;
			}

			if (clip.currentFrame == clip.frames.size()) {
				clip.currentFrame = 0;

				if (!clip.shouldLoop)
					clip.playing = false;
			}
		}
	}
};

struct SpriteInitializer {
	static void OnSpriteAttach(entt::registry& registry, entt::entity entity) {
		auto& transform = registry.get<Component::Transform>(entity);
		auto& sprite	= registry.get<Component::Sprite>(entity);

		transform.size = sprite.specs.sizeInTiles;
	}
};

}  // namespace Atlas::System