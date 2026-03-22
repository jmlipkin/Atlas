#include <Atlas.h>

#include "Atlas/Core/Application.h"
#include "EditorLayer.h"

#include "TestScript.h"

namespace Atlas {

class AtlasEditorApp : public Application {
  public:
	AtlasEditorApp() : Application(WindowProperties{"Atlas Editor"}) {
		Application::setRunMode(RunMode::BUILD);
		pushLayer(m_layer = new EditorLayer());
	}

	~AtlasEditorApp() {
		popLayer(m_layer);
	}

  private:
	Layer* m_layer;
};

Application* createApplication() {
	return new AtlasEditorApp;
}

}  // namespace Atlas
