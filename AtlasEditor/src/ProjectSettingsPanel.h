#pragma once

#include "Atlas/Scene/Scene.h"

namespace Atlas {
class ProjectSettingsPanel {
  public:
	void setProject();
	void setActiveScene(std::shared_ptr<Scene> scene) { m_scene = scene; }

	void onImGuiRender(bool& isVisible);

  private:
	bool		exists = false;
	std::string m_projName;
	int			m_tileSize;
	int			m_pixelsPerUnit;

	std::shared_ptr<Scene> m_scene;
};
}  // namespace Atlas