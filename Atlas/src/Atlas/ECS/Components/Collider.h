#pragma once

#include <glm/glm.hpp>

#include <string>
#include <cstdint>

namespace Atlas {

enum class ColliderShape {
	AABB,
	CIRCLE
};

enum class CollisionResponse {
	NONE,
	DISCRETE
};

union ColliderSize {
	glm::vec2 AABB;
	float	  radius;
};

namespace Component {

struct Collider {
	ColliderShape shape		= ColliderShape::AABB;
	glm::vec2	  offset	= {0.0f, 0.0f};
	ColliderSize  size		= {glm::vec2(1.0f)};
	bool		  isTrigger = false;
	uint32_t	  layerMask = 0xFFFFFFFF;

	std::string shapeString() const { return (shape == ColliderShape::AABB) ? "AABB" : "Circle"; }
};

struct RigidBody {
	glm::vec2		  velocity		 = {0.0f, 0.0f};
	bool			  isStatic		 = false;
	CollisionResponse responseType = CollisionResponse::NONE;
};

}  // namespace Component

}  // namespace Atlas