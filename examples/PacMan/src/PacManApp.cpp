#include <Atlas.h>

#include <Atlas/ImGui/ImGuiLayer.h>

class ExampleLayer : public Atlas::Layer {
    public:
    ExampleLayer() : Layer("Example") {}

    void onUpdate() override {
        if(Atlas::Input::isKeyPressed(AT_KEY_TAB)) {
            AT_TRACE("Tab Key Pressed! (poll)");
        }
    }

    void onEvent(Atlas::Event& event) override {
        if (event.getEventType() == Atlas::EventType::KEY_PRESSED) {
            Atlas::KeyPressedEvent& e = (Atlas::KeyPressedEvent&)event;

            if(e.getKeyCode() == AT_KEY_TAB){
                AT_TRACE("Tab key pressed (event)");
            }
            else{
                AT_TRACE("Keycode: {0} ({1})", e.getKeyCode(), (char)e.getKeyCode());
            }
        }
    }

    void onAttach() override {}
    void onDetach() override {}
};

class PacManImGui : public Atlas::ImGuiLayer {
    public:
    PacManImGui() {}

    virtual void onImGuiRender() override {
        glm::vec4 triangleColor = glm::vec4{1.0f};
        ImGui::Begin("Triangle Color");
        ImGui::ColorEdit4("Color", &triangleColor[0]);
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