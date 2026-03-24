#pragma once

#include "Atlas/Core/Log.h"
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
				break;
			default:
				AT_CORE_ERROR("Unknown collider shape");
		}
	}
};
}  // namespace Atlas