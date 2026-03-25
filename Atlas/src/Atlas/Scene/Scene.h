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
	using EventCallbackFn = std::function<void(Event&)>;

	Scene(const std::string& name);
	virtual ~Scene() = default;

	virtual void serializeData(json& data) {}
	virtual void deserializeData(const json& data) {}

	Entity	  createEntity(const std::string& name, UUID id = UUID{});
	Registry& getRegistry() { return m_registry; }

	void onEvent(Event& event);
	void onUpdate(DeltaTime dt);

	void setEventCallback(const EventCallbackFn& callback) { m_eventCallback = callback; }

	void dispatchEvent(Event& event) {
		AT_CORE_ASSERT(m_eventCallback, "Scene event callback is not set!");
		m_eventCallback(event);
	}

	std::string& getName() { return m_name; }
	std::string& getPath() { return m_filepath; }

  protected:
	virtual void onPreRender(DeltaTime dt) {}
	virtual void onRender(DeltaTime dt) {}
	virtual void onPostRender(DeltaTime dt) {}
	virtual void onEventCustom(Event& event) {}

  protected:
	Registry m_registry;
	friend class Entity;

	std::string m_name;
	std::string m_filepath;

	EventCallbackFn m_eventCallback;
};

}  // namespace Atlas