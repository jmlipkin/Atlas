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

   private:
    std::shared_ptr<SandboxScene> m_scene;
};

class SandboxImGuiLayer : public Atlas::ImGuiLayer {
   public:
    ~SandboxImGuiLayer() = default;
	virtual void onImGuiRender(Atlas::DeltaTime dt) override {
		void* data = Atlas::Application::get().getFramebuffer()->getColorTexture(0);
		ImGui::Begin("Viewport");

		ImGui::Image(data, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
    }
};

class SandboxApp : public Atlas::Application {
   public:
    SandboxApp() {
        pushLayer(new SandboxLayer());
        pushOverlay(new SandboxImGuiLayer());
    }
};

Atlas::Application* Atlas::createApplication() {
    return new SandboxApp;
}