#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Events/Event.h"
#include "Atlas/ECS/Registry.h"

namespace Atlas {

class Entity;   // forward declaration

class Scene {
   public:
    Scene(const std::string& name) : m_name(name), m_filepath(name + ".atscene") {}
    virtual ~Scene() = default;

	Entity createEntity(const std::string& name);
	Registry& getRegistry() { return m_registry; }

    virtual void setRegistryCallbacks() {}
	virtual void onUpdate(DeltaTime dt) = 0;
	virtual void onEvent(Event& event) = 0;

    std::string& getPath() { return m_filepath; }
   protected:
    Registry m_registry;
    friend class Entity;

    std::string m_name;
    std::string m_filepath;
};

}  // namespace Atlas