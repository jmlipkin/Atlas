#pragma once

#include <Atlas.h>

#include "Atlas/Core/MenuBar.h"
#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Scene/Scene.h"

#include "SceneHierarchyPanel.h"
#include "LoggingPanel.h"

namespace Atlas {
class EditorLayer : public Layer {
  public:
	EditorLayer();

	virtual void setScene(std::shared_ptr<Scene> scene);
	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(DeltaTime dt) override;
	virtual void onEvent(Event& event) override;

	virtual void onImGuiRender() override;

  private:
  std::shared_ptr<MenuBar> m_menuBar;
	SceneHierarchyPanel* m_hierarchyPanel;
	LoggingPanel m_logger;

	OrthographicCameraController m_cameraController;
	glm::vec2 m_viewportSize;

	std::vector<std::shared_ptr<Scene>> m_scenes;
};

}  // namespace Atlas