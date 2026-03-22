#pragma once

#include "Atlas/Project/Project.h"

#include <memory>

namespace Atlas {

class ProjectPanel {
  public:
	using SceneSelectedCallback = std::function<void(std::shared_ptr<Scene>)>;

	void setOnSceneSelected(SceneSelectedCallback callback) { m_onSceneSelected = callback; }

	void onImGuiRender();

  private:
	bool drawSceneNode(std::string scene_path);

  private:
	std::shared_ptr<Project> m_activeProject;
	std::shared_ptr<Scene>	 m_selectionContext;
	SceneSelectedCallback	 m_onSceneSelected;
};

}  // namespace Atlas