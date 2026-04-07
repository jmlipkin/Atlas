#pragma once

#include "Atlas/Core/Log.h"
#include "Atlas/Core/AssetManager.h"
#include "Atlas/Renderer/Renderer.h"

#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Collider.h"
#include "Atlas/ECS/Systems/Collision.h"

namespace Atlas {
class CollisionDebug {
  public:
	static void drawColliders(std::shared_ptr<Scene> scene) {
		auto view = scene->getRegistry().view<Component::Collider, Component::Transform>();
		for (auto& e : view) {
			Entity entity(e, scene.get());
			drawCollider(entity);
		}
	}

	static void drawCollider(Entity& entity) {
		if (!entity.hasComponent<Component::Collider>() || !entity.hasComponent<Component::Transform>())
			return;

		Component::Transform& t		= entity.getComponent<Component::Transform>();
		Component::Collider&  c		= entity.getComponent<Component::Collider>();
		glm::vec4			  color = {1.0f, 0.3f, 0.3f, 1.0f};

		switch (c.shape) {
			case ColliderShape::AABB:
				Renderer::drawRectangle({t.position.x + c.offset.x, t.position.y + c.offset.y}, c.size.AABB, t.position.z, color, 1.0f / 8.0f);

				if (System::Collision::currentlyColliding.contains((entt::entity(entity)))) {
					Renderer::drawQuad({t.position.x + c.offset.x, t.position.y + c.offset.y, t.position.z + 1.0f}, c.size.AABB, {color.r, color.g, color.b, 0.4f});
				}
				break;
			case ColliderShape::CIRCLE:
				Renderer::drawCircle(
					{t.position.x + c.offset.x + (t.size.x * 0.5f),
					 t.position.y + c.offset.y + (t.size.y * 0.5f)},
					c.size.radius, t.position.z + 1.0f, color, 1.0f / 8.0f);
				if (System::Collision::currentlyColliding.contains((entt::entity(entity)))) {
					Renderer::drawFilledCircle({t.position.x + c.offset.x + (t.size.x * 0.5f),
												t.position.y + c.offset.y + (t.size.y * 0.5f), t.position.z + 1.0f},
											   c.size.radius, {color.r, color.g, color.b, 0.4f});
				}
				break;
			default:
				AT_CORE_ERROR("Unknown collider shape");
		}
	}

	static void drawTilemapColliders(std::shared_ptr<Scene> scene) {
		if (!ProjectManager::getActiveProject()) return;

		int	  tileSize		= ProjectManager::getActiveProject()->getData().tileSize;
		float pixelsPerUnit = ProjectManager::getActiveProject()->getData().pixelsPerUnit;
		float tileWorldSize = tileSize / pixelsPerUnit;

		glm::vec2 tileAABB = glm::vec2(tileWorldSize);

		auto tilemaps  = scene->getRegistry().view<Component::Tilemap, Component::Transform>();
		auto colliders = scene->getRegistry().view<Component::Collider, Component::Transform>();

		for (auto e2 : tilemaps) {
			Entity				  tilemapEntity(e2, scene.get());
			Component::Tilemap&	  map  = tilemapEntity.getComponent<Component::Tilemap>();
			Component::Transform& tMap = tilemapEntity.getComponent<Component::Transform>();

			auto tileset = AssetManager::get<Tileset>(map.tileset);
			if (!tileset) continue;

			// Draw all solid tiles in muted orange
			glm::vec4 solidColor = {1.0f, 0.6f, 0.1f, 1.0f};
			glm::vec4 hitColor	 = {1.0f, 0.2f, 0.8f, 1.0f};  // magenta = actively colliding

			for (int row = 0; row < map.size.y; ++row) {
				for (int col = 0; col < map.size.x; ++col) {
					int tileIndex = map.getTile(col, row);
					if (tileIndex == -1) continue;

					TileDefinition& def = tileset->getTile(tileIndex);
					if (!def.isSolid) continue;

					glm::vec2 tileWorldPos = {
						tMap.position.x + col * tileWorldSize,
						tMap.position.y + row * tileWorldSize};

					// Check if any collider's narrowphase window covers this tile
					bool inActiveWindow = false;
					for (auto e1 : colliders) {
						Entity				  collidable(e1, scene.get());
						Component::Collider&  c = collidable.getComponent<Component::Collider>();
						Component::Transform& t = collidable.getComponent<Component::Transform>();

						glm::vec2 localPos = glm::vec2(t.position) - glm::vec2(tMap.position);

						int colMin, colMax, rowMin, rowMax;

						if (c.shape == ColliderShape::AABB) {
							// left edge is localPos.x, right edge is localPos.x + collider full width
							float left	 = localPos.x + c.offset.x;
							float right	 = left + c.size.AABB.x;
							float top	 = localPos.y + c.offset.y;
							float bottom = top + c.size.AABB.y;

							colMin = std::max((int)floor(left / tileWorldSize), 0);
							colMax = std::min((int)floor(right / tileWorldSize), map.size.x - 1);
							rowMin = std::max((int)floor(top / tileWorldSize), 0);
							rowMax = std::min((int)floor(bottom / tileWorldSize), map.size.y - 1);

						} else if (c.shape == ColliderShape::CIRCLE) {
							float cx = localPos.x + c.offset.x + c.size.radius;
							float cy = localPos.y + c.offset.y + c.size.radius;

							colMin = std::max((int)floor((cx - c.size.radius) / tileWorldSize), 0);
							colMax = std::min((int)floor((cx + c.size.radius) / tileWorldSize), map.size.x - 1);
							rowMin = std::max((int)floor((cy - c.size.radius) / tileWorldSize), 0);
							rowMax = std::min((int)floor((cy + c.size.radius) / tileWorldSize), map.size.y - 1);
						}
						if (col >= colMin && col <= colMax && row >= rowMin && row <= rowMax) {
							inActiveWindow = true;
							break;
						}
					}

					glm::vec4 color = inActiveWindow ? hitColor : solidColor;
					Renderer::drawRectangle(tileWorldPos, tileAABB, tMap.position.z, color, 1.0f / 8.0f);
				}
			}
		}
	}
};
}  // namespace Atlas