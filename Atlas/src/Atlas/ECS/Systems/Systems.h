#pragma once

#include "Atlas/Core/Time.h"

#include "Atlas/ECS/Registry.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"

namespace Atlas::System {

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