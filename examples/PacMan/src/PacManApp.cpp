#include <Atlas.h>

class ExampleLayer : public Atlas::Layer {
    public:
    ExampleLayer() : Layer("Example") {}

    void onUpdate() override {
    }

    void onEvent(Atlas::Event& event) override {
    }

    void onAttach() override {}
    void onDetach() override {}
};

class PacMan : public Atlas::Application {

    public:
    PacMan() {
        pushLayer(new ExampleLayer);
        pushOverlay(new Atlas::ImGuiLayer);
    }
    ~PacMan() override {}
};

Atlas::Application* Atlas::createApplication() {
    return new PacMan;
}