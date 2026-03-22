#include <Atlas.h>
#include "Atlas/Core/RunMode.h"
#include "Atlas/Core/Window.h"

class SandboxApp : public Atlas::Application {
  public:
	SandboxApp() : Atlas::Application({"Sandbox"}) {
		setRunMode(Atlas::RunMode::PLAY);
	}
};

Atlas::Application* Atlas::createApplication() {
    return new SandboxApp;
}