#pragma once

#include "Commands/CommandHistory.h"

#include "Atlas/Project/Project.h"

#include <memory>

namespace Atlas {

class ProjectPanel {
  public:
	using SceneSelectedCallback = std::function<void(std::shared_ptr<Scene>)>;

	ProjectPanel(CommandHistory& commandHistory) : m_commandHistory(commandHistory) {}

	void initializeContext(std::shared_ptr<Scene> scene) { m_selectionContext = scene; }
	void setOnSceneSelected(SceneSelectedCallback callback) { m_onSceneSelected = callback; }

	void onImGuiRender();

  private:
	bool drawSceneNode(std::string scene_path);

  private:
	std::shared_ptr<Project> m_activeProject;
	std::shared_ptr<Scene>	 m_selectionContext;
	SceneSelectedCallback	 m_onSceneSelected;

	CommandHistory& m_commandHistory;
};

}  // namespace Atlas