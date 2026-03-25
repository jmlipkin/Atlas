#include "atpch.h"
#include "EditorLayer.h"

#include "ProjectPanel.h"
#include "SceneHierarchyPanel.h"
#include "CollisionDebug.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Core/MenuBar.h"
#include "Atlas/Core/Time.h"
#include "Atlas/Project/Project.h"

#include "Atlas/ECS/Components/Collider.h"

#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>
#include <json/include/nlohmann/json.hpp>

namespace Atlas {

EditorLayer::EditorLayer() : Layer("Editor"), m_cameraController((float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight()) {
	m_menuBar = MenuBar::create();

	m_menuBar->setOnProjectChanged([this](std::string filepath) {
		m_config.last_open_project = filepath;
		m_projectPanel->initializeContext(m_activeScene);
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
		if (m_activeScene)
			m_hierarchyPanel->addEmptyEntity();
	});

	m_menuBar->setOnAddComponent([this]() {
		if (m_hierarchyPanel->getSelectionContext())
			m_hierarchyPanel->addComponent();
	});

	m_menuBar->setOnPreview([this]() {
		ProjectManager::saveScene(m_activeScene, Platform::getAppSupportPath() + "/preview.atscene");
		m_previewLayer = std::make_unique<RuntimeLayer>(true);
		m_previewLayer->onAttach();
		m_previewActive = true;
	});

	m_menuBar->setOnBuild([this]() {
		buildScripts();
		return !m_buildFailed;
	});

	m_menuBar->setOnValidateProjectRequired([this]() {
		return ProjectManager::getActiveProject() != nullptr;
	});

	m_menuBar->setOnValidateSceneRequired([this]() {
		return m_activeScene != nullptr && std::filesystem::path(m_activeScene->getPath()).is_absolute();
	});

	m_menuBar->setOnValidateBuildAvailable([this]() {
		return ProjectManager::getActiveProject() != nullptr && !m_isBuilding;
	});

	m_menuBar->generateMenuBar("Atlas Editor");
	m_cameraController.setZoomLevel(25.0f);

	m_projectPanel = new ProjectPanel();
	m_projectPanel->setOnSceneSelected([this](std::shared_ptr<Scene> scene) {
		setScene(scene);
	});

	m_hierarchyPanel = new SceneHierarchyPanel(nullptr);
	m_hierarchyPanel->setOnNewScript([this](Entity entity, const std::string& name) {
		m_pendingScriptAssignment = true;
		m_pendingScriptEntity	  = entity;
		m_pendingScriptName		  = name;
		buildScripts();
	});

	m_footerHeight = 12.0f * EditorWidgets::displayScale;
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
	scene->setEventCallback([this](Event& e) {
		Application::get().onEvent(e);
	});
}

void EditorLayer::onUpdate(DeltaTime dt) {
	// TODO: Fix camera controller to act differently according to game mode
	if (m_buildStatusTimer > 0.0f) {
		m_buildStatusTimer -= dt;
		if (m_buildStatusTimer <= 0.0f)
			m_buildStatus = "";
	}

	if (m_previewActive) {
		m_previewLayer->onUpdate(dt);
	} else {
		Renderer::beginScene(m_cameraController.getCamera());
		if (m_activeScene != nullptr)
			m_activeScene->onUpdate(dt);

		if (m_showAllColliders) {
			CollisionDebug::drawColliders(m_activeScene);
		}
		Entity& selected = m_hierarchyPanel->getSelectionContext();
		if (selected && selected.hasComponent<Component::Collider>()) {
			if (m_hierarchyPanel->showSelectedCollider()) {
				CollisionDebug::drawCollider(selected);
			}
		}
		Renderer::endScene();
	}
}

void EditorLayer::onEvent(Event& event) {
	m_cameraController.onEvent(event);
	if (m_activeScene != nullptr) m_activeScene->onEvent(event);
}

void EditorLayer::onImGuiRender() {
	static bool				  dockspaceOpen	  = true;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - m_footerHeight));
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

	if (m_previewActive) {
		// Don't use SetNextWindowPos/Size — let it dock naturally
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Preview", nullptr,
					 ImGuiWindowFlags_NoScrollbar |
						 ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::PopStyleVar();

		EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Green);

		if (ImGui::Button("Stop Preview")) {
			m_previewActive = false;
			m_previewLayer	= nullptr;
			std::filesystem::remove(Platform::getAppSupportPath() + "/preview.atscene");
		}

		ImVec2 previewSize = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f * EditorWidgets::displayScale);
		previewSize.x -= 3.0f * EditorWidgets::displayScale;
		void* texture = Application::get().getFramebuffer()->getColorTexture(0);
		ImGui::Image(texture, previewSize);

		ImGui::End();

		ImGui::End();  // Dockspace

		drawFooter();
		return;
	}

	// Normal editor UI
	m_logger.onImGuiRender();
	m_projectPanel->onImGuiRender();
	m_hierarchyPanel->onImGuiRender();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_viewportSize != *((glm::vec2*)&viewportSize)) {
		Application::get().getFramebuffer()->onResize(
			(uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_viewportSize = {viewportSize.x, viewportSize.y};
		m_cameraController.onResize(viewportSize.x, viewportSize.y);
	}
	void* texture = Application::get().getFramebuffer()->getColorTexture(0);
	ImGui::Image(texture, viewportSize);
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();  // Dockspace
	drawFooter();
}

void EditorLayer::loadConfig() {
	using json			 = nlohmann::ordered_json;
	std::string filepath = Platform::getAppSupportPath() + "/editor.atconfig";
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
		m_projectPanel->initializeContext(scene);
	} else if (root.contains("Last Opened Scene") && !root["Last Opened Scene"].get<std::string>().empty()) {
		auto scene = ProjectManager::loadScene(root["Last Opened Scene"].get<std::string>());
		setScene(scene);
	} else {
		setScene(std::make_shared<Scene>("New scene"));
	}
	if (root.contains("Build Directory")) {
		m_config.build_directory = root["Build Directory"];
	}
}

void EditorLayer::saveConfig() {
	using json			   = nlohmann::ordered_json;
	std::string	  filepath = Platform::getAppSupportPath() + "/editor.atconfig";
	std::ofstream file(filepath);
	AT_CORE_ASSERT(file.is_open(), "Could not open file \"{}\" for writing!", filepath);

	json root;

	if (m_config.last_open_project.empty()) {
		root["Last Opened Scene"] = m_config.last_open_scene;
	} else {
		root["Last Opened Project"] = m_config.last_open_project;
	}

	root["Build Directory"] = m_config.build_directory;

	file << root.dump(2);

	file.close();
}

void EditorLayer::drawFooter() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - m_footerHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_footerHeight));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, EditorWidgets::bg0);
	ImGui::Begin("##footer", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
					 ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoResize |
					 ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoDocking |
					 ImGuiWindowFlags_NoScrollbar);

	// Vertically center content
	float textHeight	  = ImGui::GetTextLineHeight();
	float verticalPadding = (m_footerHeight - textHeight) * 0.5f;
	ImGui::SetCursorPosY(verticalPadding);
	ImGui::Text("Atlas Editor");

	float buttonHeight	= ImGui::GetFrameHeight();
	float buttonPadding = (m_footerHeight - buttonHeight) * 0.5f;
	ImGui::SameLine();
	ImGui::SetCursorPosY(buttonPadding);
	
	ImGui::BeginDisabled(!ProjectManager::getActiveProject());
	
	if(ImGui::Button("Toggle Collider View")) {
		m_showAllColliders = !m_showAllColliders;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosY(buttonPadding);
	if (ImGui::Button("Build")) {
		buildScripts();
	}
	ImGui::EndDisabled();

	if (m_isBuilding) {
		float barHeight = ImGui::GetTextLineHeight();

		ImGui::SameLine();
		float windowY = ImGui::GetWindowPos().y;
		float centerY = windowY + (m_footerHeight - barHeight) * 0.5f;

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, centerY));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, EditorWidgets::steelBlue);
		ImGui::ProgressBar(m_buildProgress, ImVec2(50.0f * EditorWidgets::displayScale, barHeight), "");
		ImGui::PopStyleColor();

		float		textX = ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x;
		ImDrawList* dl	  = ImGui::GetWindowDrawList();
		dl->AddText(
			ImVec2(textX, centerY),
			ImGui::ColorConvertFloat4ToU32(EditorWidgets::textPrimary),
			"Building...");
	}

	if (m_buildStatusTimer > 0.0f) {
		float		centerY = ImGui::GetWindowPos().y + (m_footerHeight - ImGui::GetTextLineHeight()) * 0.5f;
		float		textX	= ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x;
		ImDrawList* dl		= ImGui::GetWindowDrawList();
		dl->AddText(
			ImVec2(textX, centerY),
			ImGui::ColorConvertFloat4ToU32(m_buildFailed ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : EditorWidgets::textPrimary),
			m_buildStatus.c_str());
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void EditorLayer::buildScripts() {
	if (m_isBuilding) return;
	if (!ProjectManager::getActiveProject()) return;

	m_isBuilding  = true;
	m_buildStatus = "Building scripts...";
	m_buildFailed = false;

	std::string buildDir = m_config.build_directory;
	std::string target	 = ProjectManager::getActiveProject()->getData().scripts_target;

	AT_CORE_DEBUG("Building target: {} in dir: {}", target, buildDir);

	Platform::buildScriptLibrary(
		buildDir,
		target,
		// onOutput — streams build output to log panel
		[this](const std::string& line) {
			AT_CORE_DEBUG("{}", line);
			float num, denom;
			std::sscanf(line.c_str(), "[%f/%f]", &num, &denom);
			m_buildProgress = (num + 1) / denom;
		},
		// onComplete — fires when cmake exits
		[this](bool success) {
			m_isBuilding	   = false;
			m_buildFailed	   = !success;
			m_buildStatus	   = success ? "Build succeeded" : "Build failed";
			m_buildStatusTimer = 3.0f;

			if (success) {
				ProjectManager::loadScriptLibrary();

				if (m_activeScene) {
					auto view = m_activeScene->getRegistry().view<Component::Script>();
					for (auto e : view) {
						auto& script = view.get<Component::Script>(e);
						if (!script.instance) {
							Entity entity(e, m_activeScene.get());
							ScriptRegistry::reinstantiateScript(script, entity);
						}
					}
				}

				if (m_pendingScriptAssignment && ScriptRegistry::isRegistered(m_pendingScriptName)) {
					auto& script = m_pendingScriptEntity.getComponent<Component::Script>();
					script.name	 = m_pendingScriptName;
					ScriptRegistry::reinstantiateScript(script, m_pendingScriptEntity);
					m_pendingScriptAssignment = false;
					m_pendingScriptName		  = {};
					ProjectManager::saveScene(m_activeScene);
				}

				AT_CORE_DEBUG("Script library reloaded successfully");
			} else {
				AT_CORE_ERROR("Script build failed — check log for details");
			}
		});
}

}  // namespace Atlas