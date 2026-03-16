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

		ImGuiIO io = ImGui::GetIO();
		ImGui::PushFont(io.Fonts->Fonts[1]);
		if (m_activeProject == nullptr) {
			ImGui::Text("No active project");
			ImGui::PopFont();
			ImGui::End();
			return;
		}
		
		std::string projName = m_activeProject->getName();
		ImGui::Text("%s", projName.c_str());
		ImGui::PopFont();

		for (auto s : m_activeProject->getData().scene_filepaths) {
			std::string displayName = std::filesystem::path(s).stem().string();

			bool isSelected = m_selectionContext && ProjectManager::toAbsolutePath(s) == m_selectionContext->getPath();

			if (ImGui::Selectable(displayName.c_str(), isSelected)) {
				auto scene = ProjectManager::loadScene(ProjectManager::toAbsolutePath(s));
				m_selectionContext = scene;
				m_onSceneSelected(scene);
			}
		}

		ImGui::End();
	}

  private:
	std::shared_ptr<Project> m_activeProject;
	std::shared_ptr<Scene>	 m_selectionContext;
	SceneSelectedCallback	 m_onSceneSelected;
};

}  // namespace Atlas