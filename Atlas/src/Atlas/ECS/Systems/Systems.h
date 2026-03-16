#pragma once

#include "Atlas/Core/Time.h"

#include "Atlas/ECS/Registry.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"

namespace Atlas::System {

struct Animation {
    static void updateFrames(Registry& registry, Atlas::DeltaTime dt) {
        auto view = registry.view<Component::Sprite, Component::Animation>();
        for (auto entity : view) {
            Component::Sprite& sprite = view.get<Component::Sprite>(entity);
            Component::Animation& animation = view.get<Component::Animation>(entity);

            if(!animation.playing)
                continue;
                
            // sprite.subtexture = animation.clip->frames[animation.nextFrame];
            AT_CORE_WARN("Animation updating is broken for now");
            // AS = 0.5 = 15fps
            // AS = 1.0 = 30fps
            // AS = 2.0 = 60fps
            animation.timeSinceLastFrame += dt;
            if(animation.timeSinceLastFrame >= 1.0f / (animation.animationSpeed * 30.0f)) {
                animation.nextFrame++;
                animation.timeSinceLastFrame = 0.0f;
            }

            if (animation.nextFrame == animation.clip->frames.size()) {
                animation.nextFrame = 0;

                if(!animation.shouldLoop)
                    animation.playing = false;
            }
        }
    }
};

struct SpriteInitializer {
    static void OnSpriteAttach(entt::registry& registry, entt::entity entity) {
        auto& transform = registry.get<Component::Transform>(entity);
        auto& sprite = registry.get<Component::Sprite>(entity);

        transform.size = sprite.specs.sizeInTiles;
    }
};

}  // namespace Atlas::System