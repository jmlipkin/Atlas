#pragma once

#include "Atlas/Core/ScriptRegistry.h"

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Components/Collider.h"

#include <json/include/nlohmann/json.hpp>

#define KEY static constexpr const char*

// The canonical component list — used by serializeEntity and deserializeEntity
#define ATLAS_ALL_COMPONENTS   \
	Component::Transform,      \
		Component::Sprite,     \
		Component::Script,     \
		Component::Collider,   \
		Component::RigidBody,  \
		Component::Animations, \
		Component::Tilemap

namespace Atlas {

using json = nlohmann::ordered_json;

template <typename T>
struct ComponentSerializer {
	KEY key() = delete;

	static void serialize(Entity entity, json& dst)			 = delete;
	static void deserialize(Entity& entity, const json& src) = delete;
};

template <typename... Components>
void serializeComponents(Entity entity, json& dst) {
	([&] {
		if (entity.hasComponent<Components>()) {
			json c;
			ComponentSerializer<Components>::serialize(entity, c);
			dst[ComponentSerializer<Components>::key()] = c;
		}
	}(),
	 ...);
}

template <typename... Components>
void deserializeComponents(Entity& entity, const json& src) {
	([&] {
		if (src.contains(ComponentSerializer<Components>::key())) {
			json component = src[ComponentSerializer<Components>::key()];
			ComponentSerializer<Components>::deserialize(entity, component);
		}
	}(),
	 ...);
}

template <>
struct ComponentSerializer<Component::Transform> {
	KEY key() { return "Transform"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::Sprite> {
	KEY key() { return "Sprite"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::Collider> {
	KEY key() { return "Collider"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::RigidBody> {
	KEY key() { return "RigidBody"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::Animations> {
	KEY key() { return "Animations"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::Tilemap> {
	KEY key() { return "Tilemap"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

template <>
struct ComponentSerializer<Component::Script> {
	KEY key() { return "Script"; }

	static void serialize(Entity entity, json& dst);
	static void deserialize(Entity& entity, const json& src);
};

}  // namespace Atlas