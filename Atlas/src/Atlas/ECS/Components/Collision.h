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
	ColliderShape shape;
	glm::vec2	  offset;
	ColliderSize  size;
	bool		  isTrigger;
	uint32_t	  layerMask;

	std::string shapeString() const { return (shape == ColliderShape::AABB) ? "AABB" : "Circle"; }
};

struct RigidBody {
	glm::vec2		  velocity;
	bool			  isStatic;
	CollisionResponse responseType;
};

}  // namespace Component

}  // namespace Atlas