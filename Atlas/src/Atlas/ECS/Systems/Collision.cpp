#include "atpch.h"
#include "Collision.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/Tileset.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Collider.h"

namespace Atlas {

CollisionEvent::CollisionEvent(Entity entityA, Entity entityB, glm::vec2 collisionNormal, float depth, bool isTrigger) : m_entityA(entityA), m_entityB(entityB), m_normal(collisionNormal), m_depth(depth), m_isTrigger(isTrigger) {}

namespace System {

void Collision::update(DeltaTime dt, Scene& scene) {
	currentlyColliding.clear();

	if (!ProjectManager::getActiveProject()) {
		return;
	}

	// TODO: Switch to cell-based iterations
	checkTilemapCollisions(dt, scene);

	auto view = scene.getRegistry().view<Component::Collider, Component::Transform>();
	for (auto it1 = view.begin(); it1 != view.end(); ++it1) {
		auto it2 = it1;
		++it2;	// start at next element (avoids duplicates + self)

		Entity entityA(*it1, &scene);
		for (; it2 != view.end(); ++it2) {
			Entity entityB(*it2, &scene);

			Component::Collider& cA = entityA.getComponent<Component::Collider>();
			Component::Collider& cB = entityB.getComponent<Component::Collider>();

			if ((cA.layerMask & cB.layerMask) == 0) {
				continue;
			}

			Component::Transform& tA = entityA.getComponent<Component::Transform>();
			Component::Transform& tB = entityB.getComponent<Component::Transform>();

			int circleCount = 0;
			circleCount += (cA.shape == ColliderShape::CIRCLE) ? 1 : 0;
			circleCount += (cB.shape == ColliderShape::CIRCLE) ? 1 : 0;

			std::optional<CollisionManifold> collision;
			switch (circleCount) {
				case 0:
					collision = compareAABBtoAABB(cA, cB, tA, tB);
					break;
				case 1:
					if (cA.shape == ColliderShape::AABB) {
						collision = compareAABBtoCircle(cA, tA, cB, tB);
					} else {
						collision = compareAABBtoCircle(cB, tB, cA, tA);
					}
					break;
				case 2:
					collision = compareCircletoCircle(cA, cB, tA, tB);
			}

			if (collision) {
				CollisionEvent event(entityA, entityB, collision->normal, collision->depth, cA.isTrigger || cB.isTrigger);
				scene.dispatchEvent(event);

				if (!event.isTrigger()) {
					resolveCollision(entityA, entityB, collision.value());
				}

				currentlyColliding.insert(entityA);
				currentlyColliding.insert(entityB);
			}
		}
	}
}

void Collision::checkTilemapCollisions(DeltaTime dt, Scene& scene) {
	auto colliders = scene.getRegistry().view<Component::Collider, Component::Transform>();
	auto tilemaps  = scene.getRegistry().view<Component::Tilemap, Component::Transform>();

	int	  tileSize		= ProjectManager::getActiveProject()->getData().tileSize;
	float pixelsPerUnit = ProjectManager::getActiveProject()->getData().pixelsPerUnit;

	float tileWorldSize = tileSize / pixelsPerUnit;

	Component::Collider tileCollider{
		ColliderShape::AABB,
		glm::vec2(0.0f),
		{glm::vec2(tileWorldSize)},
		false,
		0xFFFFFFFF};

	for (auto it1 = colliders.begin(); it1 != colliders.end(); it1++) {
		Entity collidable(*it1, &scene);

		if (!collidable.hasComponent<Component::RigidBody>()) continue;

		Component::RigidBody& rb = collidable.getComponent<Component::RigidBody>();
		if (rb.isStatic || rb.responseType == CollisionResponse::NONE) continue;

		for (auto it2 = tilemaps.begin(); it2 != tilemaps.end(); it2++) {
			Entity tilemap(*it2, &scene);

			Component::Collider& collider = collidable.getComponent<Component::Collider>();
			Component::Tilemap&	 map	  = tilemap.getComponent<Component::Tilemap>();

			if ((collider.layerMask & map.layerMask) == 0) {
				continue;
			}

			std::shared_ptr<Tileset> tileset = AssetManager::get<Tileset>(map.tileset);
			if (!tileset) continue;

			Component::Transform& tCollider = collidable.getComponent<Component::Transform>();
			Component::Transform& tMap		= tilemap.getComponent<Component::Transform>();

			glm::vec2 halfSize = (collider.shape == ColliderShape::CIRCLE)
									 ? glm::vec2(collider.size.radius)
									 : collider.size.AABB * 0.5f;

			glm::vec2 localPos = glm::vec2(tCollider.position) - glm::vec2(tMap.position);

			int colMin = 0, colMax = -1, rowMin = 0, rowMax = -1;

			if (collider.shape == ColliderShape::AABB) {
				// left edge is localPos.x, right edge is localPos.x + collider full width
				float left	 = localPos.x + collider.offset.x;
				float right	 = left + collider.size.AABB.x;
				float top	 = localPos.y + collider.offset.y;
				float bottom = top + collider.size.AABB.y;

				colMin = std::max((int)floor(left / tileWorldSize), 0);
				colMax = std::min((int)floor(right / tileWorldSize), map.size.x - 1);
				rowMin = std::max((int)floor(top / tileWorldSize), 0);
				rowMax = std::min((int)floor(bottom / tileWorldSize), map.size.y - 1);

			} else if (collider.shape == ColliderShape::CIRCLE) {
				float cx = localPos.x + collider.offset.x + collider.size.radius;
				float cy = localPos.y + collider.offset.y + collider.size.radius;

				colMin = std::max((int)floor((cx - collider.size.radius) / tileWorldSize), 0);
				colMax = std::min((int)floor((cx + collider.size.radius) / tileWorldSize), map.size.x - 1);
				rowMin = std::max((int)floor((cy - collider.size.radius) / tileWorldSize), 0);
				rowMax = std::min((int)floor((cy + collider.size.radius) / tileWorldSize), map.size.y - 1);
			}

			// Accumulate the largest correction per axis across all solid tiles,
			// then apply once. This prevents adjacent tiles from double-counting
			// overlap and stops corner oscillation.
			glm::vec2 totalCorrection(0.0f);
			glm::vec2 totalVelocityKill(0.0f);
			bool	  anyCollision = false;

			for (int row = rowMin; row <= rowMax; ++row) {
				for (int col = colMin; col <= colMax; ++col) {
					int tileIndex = map.getTile(col, row);

					if (tileIndex == -1) continue;

					TileDefinition& def = tileset->getTile(tileIndex);
					if (!def.isSolid) continue;

					tileCollider.layerMask = map.layerMask;

					Component::Transform tileTransform;
					tileTransform.position = tMap.position + glm::vec3(col, row, 0.0f) * tileWorldSize;

					std::optional<CollisionManifold> collision =
						(collider.shape == ColliderShape::AABB)
							? compareAABBtoAABB(collider, tileCollider, tCollider, tileTransform)
							: compareAABBtoCircle(tileCollider, tileTransform, collider, tCollider);

					if (!collision || collision->depth <= 0.0001f) {
						continue;
					}

					CollisionEvent event(
						collidable, tilemap,
						collision->normal, collision->depth,
						tileCollider.isTrigger || collider.isTrigger);
					scene.dispatchEvent(event);

					if (event.isTrigger()) continue;

					glm::vec2 correction = collision->normal * collision->depth;
					if (std::abs(correction.x) > std::abs(totalCorrection.x)) {
						totalCorrection.x = correction.x;
					}
					if (std::abs(correction.y) > std::abs(totalCorrection.y)) {
						totalCorrection.y = correction.y;
					}

					totalVelocityKill += collision->normal * glm::dot(rb.velocity, collision->normal);

					anyCollision = true;
				}
			}

			if (anyCollision) {
				glm::vec3& pos = tCollider.position;
				pos.x += totalCorrection.x;
				pos.y += totalCorrection.y;
				rb.velocity -= totalVelocityKill;
				currentlyColliding.insert(collidable);
			}
		}
	}
}

void Collision::resolveCollision(Entity& entityA, Entity& entityB, CollisionManifold manifold) {
	if (!entityA.hasComponent<Component::RigidBody>() || !entityB.hasComponent<Component::RigidBody>()) {
		return;
	}

	Component::RigidBody& rbA = entityA.getComponent<Component::RigidBody>();
	Component::RigidBody& rbB = entityB.getComponent<Component::RigidBody>();

	if (rbA.responseType == CollisionResponse::NONE && rbB.responseType == CollisionResponse::NONE) {
		return;
	}

	glm::vec3& posA = entityA.getComponent<Component::Transform>().position;
	glm::vec3& posB = entityB.getComponent<Component::Transform>().position;

	glm::vec2 delta = glm::vec2(posA) - glm::vec2(posB);
	if (glm::dot(delta, manifold.normal) < 0.0f) {
		manifold.normal = -manifold.normal;
	}

	int		  numStatics = 2 - ((int)rbA.isStatic + (int)rbB.isStatic);
	glm::vec2 reflection = manifold.normal * manifold.depth;

	glm::vec2 aChange = (rbA.isStatic) ? glm::vec2(0) : reflection / (float)numStatics;
	glm::vec2 bChange = (rbB.isStatic) ? glm::vec2(0) : reflection / (float)numStatics;

	rbA.velocity -= glm::dot(rbA.velocity, manifold.normal) * manifold.normal;
	rbB.velocity -= glm::dot(rbB.velocity, manifold.normal) * manifold.normal;

	posA = {aChange.x + posA.x, aChange.y + posA.y, posA.z};
	posB = {-bChange.x + posB.x, -bChange.y + posB.y, posB.z};
}

std::optional<CollisionManifold> Collision::compareAABBtoAABB(Component::Collider& cA, Component::Collider& cB, Component::Transform& tA, Component::Transform& tB) {
	glm::vec2 halfA = cA.size.AABB * 0.5f;
	glm::vec2 halfB = cB.size.AABB * 0.5f;

	glm::vec2 centerA = glm::vec2(tA.position) + cA.offset + halfA;
	glm::vec2 centerB = glm::vec2(tB.position) + cB.offset + halfB;

	glm::vec2 delta	   = centerA - centerB;
	float	  overlapX = (halfA.x + halfB.x) - std::abs(delta.x);
	float	  overlapY = (halfA.y + halfB.y) - std::abs(delta.y);

	if (overlapX <= 0 || overlapY <= 0) {
		return std::nullopt;
	}

	glm::vec2 normal(0.0f);
	float	  depth = 0.0f;

	if (overlapX < overlapY) {
		normal.x = (delta.x < 0) ? -1.0f : 1.0f;
		depth	 = overlapX;
	} else {
		normal.y = (delta.y < 0) ? -1.0f : 1.0f;
		depth	 = overlapY;
	}

	return CollisionManifold{normal, depth};
}

std::optional<CollisionManifold> Collision::compareAABBtoCircle(Component::Collider& cAABB, Component::Transform& tAABB, Component::Collider& cCircle, Component::Transform& tCircle) {
	glm::vec2 halfA = cAABB.size.AABB * 0.5f;

	glm::vec2 centerA = glm::vec2(tAABB.position) + cAABB.offset + halfA;
	glm::vec2 centerB = glm::vec2(tCircle.position) + cCircle.offset + (glm::vec2(tCircle.size) * 0.5f);

	// clamp circle center to AABB bounds to find closest point on AABB
	glm::vec2 closest = glm::clamp(centerB, centerA - halfA, centerA + halfA);

	// vector from closest point to circle center
	glm::vec2 diff	 = centerB - closest;
	float	  distSq = glm::dot(diff, diff);

	if (distSq >= cCircle.size.radius * cCircle.size.radius) {
		return std::nullopt;
	}

	float dist	= std::sqrt(distSq);
	float depth = cCircle.size.radius - dist;

	// if circle center is inside the AABB, dist will be 0 — handle that case
	glm::vec2 normal = (dist > 0.0f) ? (diff / dist) : glm::vec2(0.0f, 1.0f);

	return CollisionManifold{normal, depth};
}

std::optional<CollisionManifold> Collision::compareCircletoCircle(Component::Collider& cA, Component::Collider& cB, Component::Transform& tA, Component::Transform& tB) {
	glm::vec2 centerA = glm::vec2(tA.position) + cA.offset + (glm::vec2(tA.size) * 0.5f);
	glm::vec2 centerB = glm::vec2(tB.position) + cB.offset + (glm::vec2(tB.size) * 0.5f);

	glm::vec2 diff	 = centerA - centerB;
	float	  distSq = glm::dot(diff, diff);
	float	  radSum = cA.size.radius + cB.size.radius;

	if (distSq >= radSum * radSum) {
		return std::nullopt;
	}

	float	  dist	 = std::sqrt(distSq);
	float	  depth	 = radSum - dist;
	glm::vec2 normal = (dist > 0.0f) ? (diff / dist) : glm::vec2(0.0f, 1.0f);

	return CollisionManifold{normal, depth};
}

}  // namespace System

}  // namespace Atlas