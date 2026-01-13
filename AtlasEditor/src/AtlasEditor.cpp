#include <Atlas.h>

#include "EditorLayer.h"

namespace Atlas {

class AtlasEditorApp : public Application {
    public:
     AtlasEditorApp() : Application(WindowProperties{"Atlas Editor"}) {
         pushLayer(new EditorLayer());
     }
};

Application* createApplication() {
    return new AtlasEditorApp;
}

}  // namespace Atlas
