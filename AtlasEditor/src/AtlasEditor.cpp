#include <Atlas.h>
#include <memory>

#include "EditorLayer.h"
#include "Atlas/Core/MenuBar.h"

namespace Atlas {

class AtlasEditorApp : public Application {
    public:
     AtlasEditorApp() : Application(WindowProperties{"Atlas Editor"}) {
		 pushLayer(new EditorLayer());

		 std::shared_ptr<MenuBar> menu = MenuBar::create();
		 menu->generateMenuBar("Atlas Editor");
     }
};

Application* createApplication() {
    return new AtlasEditorApp;
}

}  // namespace Atlas
