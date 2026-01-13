#pragma once

#include "Atlas/ECS/Registry.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

namespace Atlas::System {

struct SpriteInitializer {
    static void OnSpriteAttach(entt::registry& registry, entt::entity entity) {
        auto& transform = registry.get<Component::Transform>(entity);
        auto& sprite = registry.get<Component::Sprite>(entity);

        transform.size = sprite.subtexture->getSizeInTiles();
    }
};

}  // namespace Atlas::System