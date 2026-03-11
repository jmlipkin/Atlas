#pragma once

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Core/Time.h"

#include <memory>
#include <unordered_map>

namespace Atlas {

class Behavior {
  public:
	typedef enum class PropertyType {
		BOOL,
		CHAR,
		STRING,
		INT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT4

	} PropertyType;

	struct Property {
		std::string	 name;
		void*		 valuePtr;
		PropertyType type;
	};

  public:
	virtual ~Behavior() = default;

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
};

}  // namespace Component

template <typename T>
void Entity::addScript() {
	auto& script = addComponent<Atlas::Component::Script>();
	script.instance = std::make_unique<T>();
	script.instance->setEntity(*this);
	script.instance->onCreate();
	script.instance->exposeProperties();
}

template <typename T>
Behavior::PropertyType propertyTypeFor();

template <>
inline Behavior::PropertyType propertyTypeFor<bool>() { return Behavior::PropertyType::BOOL; }
template <>
inline Behavior::PropertyType propertyTypeFor<char>() { return Behavior::PropertyType::CHAR; }
template <>
inline Behavior::PropertyType propertyTypeFor<std::string>() { return Behavior::PropertyType::STRING; }
template <>
inline Behavior::PropertyType propertyTypeFor<int>() { return Behavior::PropertyType::INT; }
template <>
inline Behavior::PropertyType propertyTypeFor<float>() { return Behavior::PropertyType::FLOAT; }
template <>
inline Behavior::PropertyType propertyTypeFor<glm::vec2>() { return Behavior::PropertyType::VEC2; }
template <>
inline Behavior::PropertyType propertyTypeFor<glm::vec3>() { return Behavior::PropertyType::VEC3; }
template <>
inline Behavior::PropertyType propertyTypeFor<glm::vec4>() { return Behavior::PropertyType::VEC4; }
template <>
inline Behavior::PropertyType propertyTypeFor<glm::mat4>() { return Behavior::PropertyType::MAT4; }

}  // namespace Atlas