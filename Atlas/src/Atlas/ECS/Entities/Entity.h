#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/ECS/Components/Components.h"

#include <entt/entt.hpp>

namespace Atlas {
class Scene;
}
namespace Atlas {

class Entity {
  public:
	Entity() : m_atlasID(UUID::null()) {}
	Entity(entt::entity entityID, Scene* scene) : m_entityID(entityID), m_scene(scene), m_atlasID(getComponent<Component::UUID>().id) {}
	Entity(entt::entity entityID, Scene* scene, UUID id) : m_entityID(entityID), m_scene(scene), m_atlasID(id) {}

	Entity(const Entity& other) = default;

	UUID getUUID() const { return m_atlasID; }

	inline const std::string& name() { return getComponent<Component::Tag>().tag; };

	template <typename T, typename... Args>
	T& addComponent(Args&&... args);

	template <typename T>
	void removeComponent();

	template <typename T>
	T& getComponent();

	template <typename T>
	bool hasComponent();

	// Defined in Behavior.h
	template <typename T>
	void addScript();

	operator entt::entity() { return m_entityID; }
	operator const entt::entity() const { return m_entityID; }

	bool operator==(const Entity& other) const { return m_atlasID == other.m_atlasID; }

	operator bool() const { return m_atlasID != UUID::null() && m_scene != nullptr; }

	// TEMPORARY
	// explicit operator uint32_t() { return (uint32_t)m_entityID; }

  private:
	entt::entity m_entityID;
	Scene*		 m_scene = nullptr;
	UUID		 m_atlasID;
};

}  // namespace Atlas

#include "Atlas/Scene/Scene.h"

namespace Atlas {

template <typename T, typename... Args>
T& Entity::addComponent(Args&&... args) {
	AT_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
	return m_scene->m_registry.emplace<T>(m_entityID, std::forward<Args>(args)...);
}

template <typename T>
void Entity::removeComponent() {
	AT_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
	m_scene->m_registry.remove<T>(m_entityID);
}

template <typename T>
T& Entity::getComponent() {
	AT_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
	return m_scene->m_registry.get<T>(m_entityID);
}

template <typename T>
bool Entity::hasComponent() {
	return m_scene->m_registry.any_of<T>(m_entityID);
}

}  // namespace Atlas