#pragma once

#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/ImGuiSystem.h"

#include <imgui/imgui.h>
#include <memory>

namespace Atlas {

class ProjectPanel {
  public:
	using SceneSelectedCallback = std::function<void(std::shared_ptr<Scene>)>;

	void setOnSceneSelected(SceneSelectedCallback callback) { m_onSceneSelected = callback; }

	void onImGuiRender() {
		m_activeProject = ProjectManager::getActiveProject();

		ImGui::Begin("Project");
		ImGuiSystem::DrawPanelAccentBar(ImGuiSystem::PanelAccent::Purple);

		if (m_activeProject == nullptr) {
			ImGui::Text("No active project");
			ImGui::End();
			return;
		}

		std::string projName = m_activeProject->getName();
		ImGui::Text("%s", projName.c_str());

		for (auto s : m_activeProject->getData().scene_filepaths) {
			std::string displayName = std::filesystem::path(s).stem().string();
			if (ImGui::Selectable(displayName.c_str())) {
				auto scene = ProjectManager::loadScene(ProjectManager::toAbsolutePath(s));
				m_onSceneSelected(scene);
			}
		}

		ImGui::End();
	}

  private:
	std::shared_ptr<Project> m_activeProject;
	SceneSelectedCallback	 m_onSceneSelected;
};

}  // namespace Atlas