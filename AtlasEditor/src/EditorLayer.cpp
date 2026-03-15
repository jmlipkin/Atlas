#include "atpch.h"
#include "EditorLayer.h"

#include "ProjectPanel.h"
#include "SceneHierarchyPanel.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Core/MenuBar.h"
#include "Atlas/Core/Time.h"
#include "Atlas/Project/Project.h"

#include <imgui/imgui.h>
#include <json/include/nlohmann/json.hpp>

namespace Atlas {

EditorLayer::EditorLayer() : Layer("Editor"), m_cameraController((float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight()) {
	m_menuBar = MenuBar::create();

	m_menuBar->setOnProjectChanged([this](std::string filepath) {
		m_config.last_open_project = filepath;
	});

	m_menuBar->setOnSceneSaved([this]() {
		ProjectManager::saveScene(m_activeScene);
	});

	m_menuBar->setOnSceneLoaded([this](std::shared_ptr<Scene> scene) {
		setScene(scene);
	});

	m_menuBar->setOnNewScene([this](std::shared_ptr<Scene> scene) {
		setScene(scene);
	});

	m_menuBar->setOnProjectClosed([this]() {
		setScene(std::make_shared<Scene>("New Scene"));
		m_config.last_open_project = "";
	});

	m_menuBar->setOnNewEntity([this]() {
		if(m_activeScene)
			m_hierarchyPanel->addEmptyEntity();
	});

	m_menuBar->setOnAddComponent([this]() {
		if(m_hierarchyPanel->getSelectionContext())
			m_hierarchyPanel->addComponent();
	});

	m_menuBar->generateMenuBar("Atlas Editor");
	m_cameraController.setZoomLevel(25.0f);

	m_projectPanel = new ProjectPanel();
	m_projectPanel->setOnSceneSelected([this](std::shared_ptr<Scene> scene) {
		setScene(scene);
	});

	m_hierarchyPanel = new SceneHierarchyPanel(nullptr);
}

void EditorLayer::setScene(std::shared_ptr<Scene> scene) {
	m_activeScene = scene;
	m_hierarchyPanel->setScene(scene);
	ProjectManager::setActiveScene(scene);
	if (!ProjectManager::getActiveProject() && scene) {
		if (std::filesystem::path(scene->getPath()).is_absolute()) {
			m_config.last_open_scene = scene->getPath();
		}
	}
}

void EditorLayer::onUpdate(DeltaTime dt) {
	// TODO: Fix camera controller to act differently according to game mode
	// m_cameraController.onUpdate(dt);
	Renderer::beginScene(m_cameraController.getCamera());
	if (m_activeScene != nullptr) m_activeScene->onUpdate(dt);
	Renderer::endScene();
}

void EditorLayer::onEvent(Event& event) {
	m_cameraController.onEvent(event);
	if (m_activeScene != nullptr) m_activeScene->dispatchEvent(event);
}

void EditorLayer::onImGuiRender() {
	static bool				  dockspaceOpen	  = true;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dockspace", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0, 0), dockspace_flags);

#ifndef AT_PLATFORM_MACOS
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
				Application::get().close();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
#endif

	m_logger.onImGuiRender();
	m_projectPanel->onImGuiRender();
	m_hierarchyPanel->onImGuiRender();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_viewportSize != *((glm::vec2*)&viewportSize)) {
		Application::get().getFramebuffer()->onResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_viewportSize = {viewportSize.x, viewportSize.y};
		m_cameraController.onResize(viewportSize.x, viewportSize.y);
	}
	void* texture = Application::get().getFramebuffer()->getColorTexture(0);
	ImGui::Image(texture, viewportSize);
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

void EditorLayer::loadConfig() {
	using json			 = nlohmann::ordered_json;
	std::string filepath = Platform::getResourcesPath() + "/editor.atconfig";
	if (!std::filesystem::exists(filepath)) {
		return;
	}

	std::ifstream file(filepath);
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for reading!", filepath);

	json root = json::parse(file);
	file.close();

	if (root.contains("Last Opened Project")) {
		auto scene = ProjectManager::loadProject(root["Last Opened Project"]);
		setScene(scene);
	} else if (root.contains("Last Opened Scene") && !root["Last Opened Scene"].get<std::string>().empty()) {
		auto scene = ProjectManager::loadScene(root["Last Opened Scene"].get<std::string>());
		setScene(scene);
	} else {
		setScene(std::make_shared<Scene>("New scene"));
	}
}

void EditorLayer::saveConfig() {
	using json			   = nlohmann::ordered_json;
	std::string	  filepath = Platform::getResourcesPath() + "/editor.atconfig";
	std::ofstream file(filepath);
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for writing!", filepath);

	json root;

	if (m_config.last_open_project.empty()) {
		root["Last Opened Scene"] = m_config.last_open_scene;
	} else {
		root["Last Opened Project"] = m_config.last_open_project;
	}
	file << root.dump(2);

	file.close();
}

}  // namespace Atlas