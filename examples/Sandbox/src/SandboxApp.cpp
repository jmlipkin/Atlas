#include <Atlas.h>

#include "SandboxScene.h"

class SandboxLayer : public Atlas::Layer {
   public:
    SandboxLayer() : Layer("Example") {
        m_scene = std::make_shared<SandboxScene>();
    }

    virtual void onAttach() override {}
    virtual void onDetach() override {}
    virtual void onUpdate(Atlas::DeltaTime dt) override {
        m_scene->onUpdate(dt);
    }
    virtual void onEvent(Atlas::Event& event) override {
        // m_scene->onEvent(event);
    }

   private:
    std::shared_ptr<SandboxScene> m_scene;
};

class SandboxApp : public Atlas::Application {
    public:
    SandboxApp() {
        pushLayer(new SandboxLayer());
    }
};

Atlas::Application* Atlas::createApplication() {
    return new SandboxApp;
}