#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Events/Event.h"
#include "Atlas/ECS/Registry.h"

namespace Atlas {

class Entity;   // forward declaration

class Scene {
   public:
    Scene() = default;
    virtual ~Scene() = default;

	Entity createEntity(const std::string& name);
	Registry& getRegistry() { return m_registry; }

    virtual void setRegistryCallbacks() {}
	virtual void onUpdate(DeltaTime dt) = 0;
	virtual void onEvent(Event& event) = 0;

   protected:
    Registry m_registry;
    friend class Entity;
};

}  // namespace Atlas