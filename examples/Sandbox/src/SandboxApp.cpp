#include <Atlas.h>

#include "SandboxScene.h"
#include "imgui/imgui.h"

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
        m_scene->onEvent(event);
	}

    virtual void onImGuiRender() override {
		auto fbuf = Atlas::Application::get().getFramebuffer();
		void* data = fbuf->getColorTexture(0);
		ImGui::Begin("Viewport");

		ImGui::Image(data, ImVec2(fbuf->getWidth() * 3.0/4.0, fbuf->getHeight() * 3.0/4.0), ImVec2(0, 0), ImVec2(1, 1));
		ImGui::End();
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