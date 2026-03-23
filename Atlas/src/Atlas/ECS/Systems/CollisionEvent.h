#pragma once

#include "Atlas/Events/UserEvent.h"
#include "Atlas/ECS/Entities/Entity.h"

namespace Atlas {

class CollisionEvent : public UserEvent {
  public:
	CollisionEvent(Entity entityA, Entity entityB, glm::vec2 collisionNormal, float depth, bool isTrigger);
};

}  // namespace Atlas