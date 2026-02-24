#pragma once

#include "Atlas/Scene/Scene.h"

#include <entt/entt.hpp>

namespace Atlas {

class Entity {
   public:
    Entity() = default;
    Entity(entt::entity entityID, Scene* scene) : m_entityID(entityID), m_scene(scene) {}

    Entity(const Entity& other) = default;

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        AT_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
        return m_scene->m_registry.emplace<T>(m_entityID, std::forward<Args>(args)...);
    }

    template<typename T>
    void removeComponent() {
        AT_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        m_scene->m_registry.remove<T>(m_entityID);
    }

    template<typename T>
    T& getComponent() {
        AT_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        return m_scene->m_registry.get<T>(m_entityID);
    }

    template<typename T>
    bool hasComponent() {
		return m_scene->m_registry.any_of<T>(m_entityID);
    }

    operator entt::entity() { return m_entityID; }
	operator const entt::entity() const { return m_entityID; }
	bool operator ==(const Entity& other) const { return m_entityID == other.m_entityID; } 

	// TEMPORARY
	operator uint32_t() { return (uint32_t)m_entityID; }

   private:
    entt::entity m_entityID;
    Scene* m_scene = nullptr;
};

}  // namespace Atlas