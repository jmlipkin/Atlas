#pragma once

#include <Atlas.h>

#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Systems/Collision.h"
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

		glm::vec3 startPos = t.position;

		auto& animations = getComponent<Component::Animations>();
		if (Input::isKeyPressed(AT_KEY_A)) {
			t.position.x -= m_speed * dt;

			AnimationClip& left = animations.clips["Left"];
			left.playing		= true;
		}
		if (Input::isKeyPressed(AT_KEY_D)) {
			t.position.x += m_speed * dt;

			AnimationClip& right = animations.clips["Right"];
			right.playing		 = true;
		}
		if (Input::isKeyPressed(AT_KEY_W)) {
			t.position.y -= m_speed * dt;

			AnimationClip& up = animations.clips["Up"];
			up.playing		  = true;
		}
		if (Input::isKeyPressed(AT_KEY_S)) {
			t.position.y += m_speed * dt;

			AnimationClip& down = animations.clips["Down"];
			down.playing		= true;
		}

		glm::vec2 delta = t.position - startPos;

		if (delta.x == 0 && delta.y == 0) {
			animations.activeClip = "";
		} else if (std::abs(delta.x) > std::abs(delta.y)) {
			animations.activeClip = delta.x >= 0 ? "Right" : "Left";
		} else {
			animations.activeClip = delta.y >= 0 ? "Down" : "Up";
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
		expose("Dying played", m_dyingPlayed);
		expose("Speed", m_speed);
		expose("Score", m_score);
	}

  private:
	bool onCollision(CollisionEvent& event) {
		if ((m_entity == event.getEntityA() || m_entity == event.getEntityB())) {
			if(event.getEntityA().name() == "Left Tunnel" || event.getEntityB().name() == "Left Tunnel") {
				getComponent<Component::Transform>().position.x = 13.5;
			}
			if(event.getEntityA().name() == "Right Tunnel" || event.getEntityB().name() == "Right Tunnel") {
				getComponent<Component::Transform>().position.x = -15.5;
			}
		}
		return false;
	}

  private:
	Entity m_entity;
	// add member variables here
	float m_speed = 8.0f;
	int	  m_score = 0;

	bool m_dyingPlayed = false;
};

AT_REGISTER_SCRIPT(Player)

}  // namespace Atlas
