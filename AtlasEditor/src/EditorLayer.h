#pragma once

#include <Atlas.h>

#include "AtlasPaths.h"
#include "Atlas/Core/MenuBar.h"
#include "Atlas/Core/RuntimeLayer.h"
#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Scene/Scene.h"

#include "ProjectPanel.h"
#include "ProjectSettingsPanel.h"
#include "SceneHierarchyPanel.h"
#include "LoggingPanel.h"

namespace Atlas {

struct EditorConfig {
	std::string last_open_project = "";
	std::string last_open_scene	  = "";
	std::string build_directory	  = ATLAS_DEFAULT_BUILD_DIR;
};

class EditorLayer : public Layer {
  public:
	EditorLayer();

	virtual void setScene(std::shared_ptr<Scene> scene);
	virtual void onAttach() override { loadConfig(); }
	virtual void onDetach() override { saveConfig(); }
	virtual void onUpdate(DeltaTime dt) override;
	virtual void onEvent(Event& event) override;

	virtual void onImGuiRender() override;

  private:
	void drawFooter();

	virtual void loadConfig();
	virtual void saveConfig();
	virtual void buildScripts();

  private:
	std::shared_ptr<MenuBar> m_menuBar;
	SceneHierarchyPanel*	 m_hierarchyPanel;
	ProjectPanel*			 m_projectPanel;
	LoggingPanel			 m_logger;

	OrthographicCameraController m_cameraController;
	glm::vec2					 m_viewportSize;

	std::shared_ptr<Scene> m_activeScene;

	EditorConfig m_config;

	bool						  m_previewActive = false;
	std::unique_ptr<RuntimeLayer> m_previewLayer;
	ImVec2						  m_previewSize = {0, 0};

	float m_footerHeight;
	bool m_isBuilding = false;
	std::string m_buildStatus;
	float m_buildProgress = 0.0f;
	bool m_buildFailed = false;
	float m_buildStatusTimer = 0.0f;

	bool m_pendingScriptAssignment;
	Entity m_pendingScriptEntity;
	std::string m_pendingScriptName;

	bool m_showAllColliders = false;

	ProjectSettingsPanel m_projectSettings;
	bool m_settingsActive = false;
};

}  // namespace Atlas