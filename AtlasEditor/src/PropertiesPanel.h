#pragma once

#include "Atlas/Scene/Scene.h"
#include "imgui/imgui.h"

namespace Atlas {

class PropertiesPanel {
  public:
	PropertiesPanel(std::shared_ptr<Scene> scene) : m_scene(scene) {}
	~PropertiesPanel() = default;

	void onImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		ImGui::End();
	}

  private:
	std::shared_ptr<Scene> m_scene;
};

}  // namespace Atlas