#pragma once

#include <Atlas.h>
#include "Atlas/Core/Base.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Systems/Collision.h"
#include "Atlas/ECS/Systems/Systems.h"
#include "Atlas/Events/Event.h"

namespace Atlas {

class Player : public Atlas::Behavior {
  public:
	AT_BEHAVIOR(Player)

	virtual void onCreate() override {
		m_entity = getEntity();
	}

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
	virtual void onEvent(Event& event) override {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<CollisionEvent>(AT_BIND_EVENT_FN(Player::onCollision));
	}

	virtual void exposeProperties() override {
		// Add any variables that you want visible in the editor here using the
		// expose() function.
		// Usage: expose(<variable name>, <value>);
		expose("Speed", m_speed);
	}

  private:
	bool onCollision(CollisionEvent& event) {
		if (!m_dyingPlayed && (m_entity == event.getEntityA() || m_entity == event.getEntityB())) {

            auto& clips = getComponent<Component::Animations>().clips;
			AnimationClip& dying = clips["Dying"];
			if (!dying.playing) {
				m_dyingPlayed = true;
				dying.playing = true;
                getComponent<Component::Animations>().activeClip = "Dying";
			}
		}
		return false;
	}

  private:
	Entity m_entity;
	// add member variables here
	float m_speed = 8.0f;

	bool m_dyingPlayed = false;
};

AT_REGISTER_SCRIPT(Player)

}  // namespace Atlas
