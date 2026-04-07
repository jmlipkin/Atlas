#include "atpch.h"
#include "ProjectSettingsPanel.h"

#include "Atlas/Project/Project.h"

#include <imgui/imgui.h>

namespace Atlas {
void ProjectSettingsPanel::setProject() {
	auto project = ProjectManager::getActiveProject();

	if (project) {
		exists			= true;
		m_projName		= project->getName();
		m_tileSize		= project->getData().tileSize;
		m_pixelsPerUnit = project->getData().pixelsPerUnit;
	}

	else {
		exists	   = false;
		m_projName = "No active project";
	}
}

void ProjectSettingsPanel::onImGuiRender(bool& isVisible) {
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowContentRegionMax().x * 0.5f, ImGui::GetWindowContentRegionMax().y * 0.5f));
	ImGui::Begin("Project Settings", &isVisible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	ImGui::Text("%s", m_projName.c_str());

	if (!exists) {
		ImGui::End();
		return;
	}

	ImGui::BeginChild("World Settings");

	ImGui::DragInt("Tile Size", &m_tileSize, 1.0f, 1);
	ImGui::DragInt("Pixels Per Unit", &m_pixelsPerUnit, 1.0f, 1);

	if (ImGui::Button("Apply")) {
		auto project					 = ProjectManager::getActiveProject();
		project->getData().tileSize		 = m_tileSize;
		project->getData().pixelsPerUnit = m_pixelsPerUnit;
		ProjectManager::saveProject();

		if (m_scene)
			m_scene->setTileSize(m_tileSize);
	}

	if (ImGui::Button("Cancel")) {
		isVisible = false;
	}

	ImGui::EndChild();
	ImGui::End();
}
}  // namespace Atlas