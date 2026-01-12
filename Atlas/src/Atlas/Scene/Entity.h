#pragma once

#include "Atlas/Scene/Scene.h"

#include <entt/entt.hpp>

namespace Atlas {

class Entity {
   public:
    operator entt::entity() { return m_entityID; }
    operator const entt::entity() const { return m_entityID; }

   private:
    entt::entity m_entityID;
    Scene m_scene;
};

}  // namespace Atlas