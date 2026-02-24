#pragma once

#include "Atlas/Core/Time.h"
#include "Entity.h"

namespace Atlas {

class ScriptableEntity {
  public:
	virtual ~ScriptableEntity() = default;

	template <typename T>
	T& getComponent() { return m_entity.getComponent<T>(); }

//   protected:
	virtual void onCreate() {}
	virtual void onDestroy() {}
	virtual void onUpdate(DeltaTime dt) {}
	virtual void onEvent(Event& event) {}

//   private:
	Entity m_entity;
	friend class Scene;
};

}  // namespace Atlas