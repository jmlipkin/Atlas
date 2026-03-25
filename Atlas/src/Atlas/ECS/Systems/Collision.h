#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Events/UserEvent.h"

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Collider.h"

namespace Atlas {

class CollisionEvent : public UserEvent {
  public:
	EVENT_CLASS_CATEGORY(EVENT_CATEGORY_USER);

	CollisionEvent(Entity entityA, Entity entityB, glm::vec2 collisionNormal, float depth, bool isTrigger);

	Entity& getEntityA() { return m_entityA; }
	Entity& getEntityB() { return m_entityB; }

	glm::vec2 getNormal() { return m_normal; }
	float	  getDepth() { return m_depth; }

	bool isTrigger() { return m_isTrigger; }

	EVENT_CLASS_TYPE(COLLISION);

  private:
	Entity	  m_entityA;
	Entity	  m_entityB;
	glm::vec2 m_normal;
	float	  m_depth;
	bool	  m_isTrigger;
};

namespace System {

struct CollisionManifold {
	glm::vec2 normal;
	float	  depth;
};

class Collision {
  public:
	inline static std::set<entt::entity> currentlyColliding;

	static void update(DeltaTime dt, Scene& scene);

  private:
	static std::optional<CollisionManifold> compareAABBtoAABB(Component::Collider& cA, Component::Collider& cB, Component::Transform& tA, Component::Transform& tB);
	static std::optional<CollisionManifold> compareAABBtoCircle(Component::Collider& cAABB, Component::Transform& tAABB, Component::Collider& cCircle, Component::Transform& tCircle);
	static std::optional<CollisionManifold> compareCircletoCircle(Component::Collider& cA, Component::Collider& cB, Component::Transform& tA, Component::Transform& tB);
};

}  // namespace System

}  // namespace Atlas