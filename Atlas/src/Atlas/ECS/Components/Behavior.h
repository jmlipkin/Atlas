#pragma once

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Core/Time.h"
#include "Atlas/Core/ScriptPriority.h"

#include <memory>
#include <unordered_map>

#define AT_BEHAVIOR(Type) \
	virtual std::string getTypeName() const override { return #Type; }

namespace Atlas {

enum class BehaviorPropertyType {
	BOOL,
	CHAR,
	STRING,
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT4

};

template <typename T>
BehaviorPropertyType propertyTypeFor();

class Behavior {
  public:
	struct Property {
		std::string			 name;
		void*				 valuePtr;
		BehaviorPropertyType type;
	};

  public:
	virtual ~Behavior() = default;

	virtual std::string getTypeName() const { return "UnknownBehavior"; }

	void setEntity(Entity entity) { m_entity = entity; }

	virtual void exposeProperties() {}
	const auto&	 getProperties() { return m_properties; }

	template <typename T>
	T& getComponent() { return m_entity.getComponent<T>(); }

	virtual void onCreate() {}
	virtual void onDestroy() {}
	virtual void onUpdate(DeltaTime dt) {}
	virtual void onEvent(Event& event) {}

  protected:
	template <typename T>
	void expose(const std::string& name, T& value) {
		m_properties[name] = Property{name, &value, propertyTypeFor<T>()};
	}

  private:
	Entity m_entity;
	friend class Scene;

	std::unordered_map<std::string, Property> m_properties;
};

namespace Component {

struct Script {
	std::unique_ptr<Behavior> instance = nullptr;
	ScriptPriority			  priority = ScriptPriority::Normal;
};

}  // namespace Component

}  // namespace Atlas

#include "Atlas/Core/ScriptRegistry.h"

namespace Atlas {

template <typename T>
void Entity::addScript() {
	auto& script	= addComponent<Component::Script>();
	script.instance = std::make_unique<T>();
	script.instance->setEntity(*this);
	script.instance->onCreate();
	script.instance->exposeProperties();
	script.priority = ScriptRegistry::getPriority(script.instance->getTypeName());
}

template <>
inline BehaviorPropertyType propertyTypeFor<bool>() { return BehaviorPropertyType::BOOL; }
template <>
inline BehaviorPropertyType propertyTypeFor<char>() { return BehaviorPropertyType::CHAR; }
template <>
inline BehaviorPropertyType propertyTypeFor<std::string>() { return BehaviorPropertyType::STRING; }
template <>
inline BehaviorPropertyType propertyTypeFor<int>() { return BehaviorPropertyType::INT; }
template <>
inline BehaviorPropertyType propertyTypeFor<float>() { return BehaviorPropertyType::FLOAT; }
template <>
inline BehaviorPropertyType propertyTypeFor<glm::vec2>() { return BehaviorPropertyType::VEC2; }
template <>
inline BehaviorPropertyType propertyTypeFor<glm::vec3>() { return BehaviorPropertyType::VEC3; }
template <>
inline BehaviorPropertyType propertyTypeFor<glm::vec4>() { return BehaviorPropertyType::VEC4; }
template <>
inline BehaviorPropertyType propertyTypeFor<glm::mat4>() { return BehaviorPropertyType::MAT4; }

}  // namespace Atlas