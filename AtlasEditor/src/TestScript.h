#pragma once
#include <Atlas.h>

class TestScript : public Atlas::Behavior {
  public:
	AT_BEHAVIOR(TestScript)

	virtual void onCreate() override {
	}

	virtual void onUpdate(Atlas::DeltaTime dt) override {
	}

	virtual void exposeProperties() override {
		expose("Speed", m_speed);
		expose("Active", m_active);
		expose("Name", m_name);
	}

  private:
	float		m_speed	 = 10.0f;
	bool		m_active = true;
	std::string m_name	 = "test property";
};

AT_REGISTER_SCRIPT(TestScript)