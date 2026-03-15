#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Core/UUID.h"
#include "Atlas/Events/Event.h"
#include "Atlas/ECS/Registry.h"

#include "nlohmann/json_fwd.hpp"

namespace Atlas {

class Entity;  // forward declaration
using json = nlohmann::ordered_json;

class Scene {
  public:
	Scene(const std::string& name);
	virtual ~Scene() = default;

	virtual void serializeData(json& data) {}
	virtual void deserializeData(const json& data) {}

	Entity	  createEntity(const std::string& name, UUID id = UUID{});
	Registry& getRegistry() { return m_registry; }

	// Renamed so that derived classes can call onEvent()
	void dispatchEvent(Event& event);
	void onUpdate(DeltaTime dt);

	std::string& getName() { return m_name; }
	std::string& getPath() { return m_filepath; }

  protected:
	virtual void onPreRender(DeltaTime dt) {}
	virtual void onRender(DeltaTime dt) {}
	virtual void onPostRender(DeltaTime dt) {}
	virtual void onEvent(Event& event) {}

  protected:
	Registry m_registry;
	friend class Entity;

	std::string m_name;
	std::string m_filepath;
};

}  // namespace Atlas