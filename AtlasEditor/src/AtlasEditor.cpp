#include <Atlas.h>

#include "EditorLayer.h"

namespace Atlas {

class AtlasEditorApp : public Application {
  public:
	AtlasEditorApp() : Application(WindowProperties{"Atlas Editor"}) {
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
