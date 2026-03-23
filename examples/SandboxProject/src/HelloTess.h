#pragma once

#include <Atlas.h>
#include "Atlas/ECS/Components/Components.h"

namespace Atlas {

class HelloTess : public Atlas::Behavior {
  public:
	AT_BEHAVIOR(HelloTess)

	virtual void onCreate() override {}
	virtual void onDestroy() override {}
	virtual void onUpdate(DeltaTime dt) override {
		Component::Transform& t = getComponent<Component::Transform>();

		if (Input::isKeyPressed(AT_KEY_A)) {
			t.position.x -= m_speed * dt;
		}
		if (Input::isKeyPressed(AT_KEY_D)) {
			t.position.x += m_speed * dt;
		}
		if (Input::isKeyPressed(AT_KEY_W)) {
			t.position.y -= m_speed * dt;
		}
		if (Input::isKeyPressed(AT_KEY_S)) {
			t.position.y += m_speed * dt;
		}
	}
	virtual void onEvent(Event& event) override {}

	virtual void exposeProperties() override {
		// Add any variables that you want visible in the editor here using the
		// expose() function.
		// Usage: expose(<variable name>, <value>);
		expose("Speed", m_speed);
	}

  private:
	// add member variables here
	float m_speed = 10.0f;
};

AT_REGISTER_SCRIPT(HelloTess)

}  // namespace Atlas
