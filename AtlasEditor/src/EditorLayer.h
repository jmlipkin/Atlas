#pragma once

#include <Atlas.h>

#include "Atlas/Core/MenuBar.h"
#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Scene/Scene.h"

#include "SceneHierarchyPanel.h"
#include "LoggingPanel.h"

namespace Atlas {

struct EditorConfig {
	std::string last_open_project = "";
	std::string last_open_scene	  = "";
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
	virtual void loadConfig();
	virtual void saveConfig();

  private:
	std::shared_ptr<MenuBar> m_menuBar;
	SceneHierarchyPanel*	 m_hierarchyPanel;
	LoggingPanel			 m_logger;

	OrthographicCameraController m_cameraController;
	glm::vec2					 m_viewportSize;

	std::shared_ptr<Scene> m_activeScene;

	EditorConfig m_config;
};

}  // namespace Atlas