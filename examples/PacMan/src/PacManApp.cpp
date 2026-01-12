#include <Atlas.h>

#include <Atlas/ImGui/ImGuiLayer.h>

glm::vec4 quadColor(1.0f);

class ExampleLayer : public Atlas::Layer {
    public:
    ExampleLayer() : Layer("Example"), m_cameraController(1280.0f/720.0f, true) {
        std::string tempDirectory = "/Users/jared/Documents/GameDev/Atlas/examples/PacMan/src/";
        std::string texturefile = "pacman_all.png";

        Atlas::TextureSheetSpecification specs{glm::vec2(8.0f)};
        m_textureSheet = std::make_shared<Atlas::TextureSheet>(tempDirectory + texturefile, specs);
        m_characterSheet = std::make_shared<Atlas::TextureSheet>(*m_textureSheet, glm::ivec2(57, 0), glm::ivec2(85, 31));
        m_sprite = m_characterSheet->addSubTexture("board", glm::ivec2(0, 0), glm::ivec2(2, 2));

        m_cameraController.setZoomLevel(20.0f);
        }

    void onUpdate(Atlas::DeltaTime dt) override {
        m_cameraController.onUpdate(dt);
        Atlas::Renderer::beginScene(m_cameraController.getCamera());

        // Atlas::Renderer::drawQuad(glm::vec3(0.0f), m_textureSheet);
        Atlas::Renderer::drawQuad(glm::vec2(0.0f, 32.0f), m_textureSheet);
        Atlas::Renderer::drawQuad(glm::vec2(0.0f), m_sprite);

        Atlas::Renderer::endScene();
    }

    void onEvent(Atlas::Event& event) override {
        m_cameraController.onEvent(event);
    }

    void onAttach() override {}
    void onDetach() override {}

    private:
     std::shared_ptr<Atlas::TextureSheet> m_textureSheet;
     std::shared_ptr<Atlas::TextureSheet> m_characterSheet;
     std::shared_ptr<Atlas::Texture> m_food;
     Atlas::OrthographicCameraController m_cameraController;
     std::shared_ptr<Atlas::SubTexture> m_sprite;
};

class PacManImGui : public Atlas::ImGuiLayer {
    public:
    PacManImGui() {}

    virtual void onImGuiRender(Atlas::DeltaTime dt) override {
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