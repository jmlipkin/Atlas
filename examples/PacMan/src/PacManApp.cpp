#include <Atlas.h>

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

class PacMan : public Atlas::Application {

    public:
    PacMan() {
        pushLayer(new ExampleLayer);
    }
    ~PacMan() override {}
};

Atlas::Application* Atlas::createApplication() {
    return new PacMan;
}