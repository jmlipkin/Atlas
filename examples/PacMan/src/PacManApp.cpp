#include <Atlas.h>

#include <Atlas/ImGui/ImGuiLayer.h>

glm::vec4 quadColor(1.0f);

class ExampleLayer : public Atlas::Layer {
    public:
    ExampleLayer() : Layer("Example"), m_cameraController(1280.0f/720.0f, true) {}

    void onUpdate(Atlas::DeltaTime dt) override {
        m_cameraController.onUpdate(dt);
        Atlas::Renderer::beginScene(m_cameraController.getCamera());

        Atlas::Renderer::drawQuad(glm::vec3(0.0f), glm::vec2(1.0f), quadColor);

        Atlas::Renderer::endScene();
    }

    void onEvent(Atlas::Event& event) override {
        m_cameraController.onEvent(event);
    }

    void onAttach() override {}
    void onDetach() override {}

    private:
     Atlas::OrthographicCameraController m_cameraController;
};

class PacManImGui : public Atlas::ImGuiLayer {
    public:
    PacManImGui() {}

    virtual void onImGuiRender() override {
        ImGui::Begin("Quad Data");
        ImGui::ColorPicker4("Color", &quadColor[0]);
        ImGui::End();

    }
};

class PacMan : public Atlas::Application {

    public:
    PacMan() {
        pushLayer(new ExampleLayer);
        pushOverlay(new PacManImGui);
    }
    ~PacMan() override = default;
};

Atlas::Application* Atlas::createApplication() {
    return new PacMan;
}