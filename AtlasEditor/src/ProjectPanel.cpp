#include "atpch.h"
#include "ProjectPanel.h"

#include "Atlas/Core/Platform.h"

#include "Atlas/ImGui/EditorWidgets.h"
#include <imgui/imgui.h>

namespace Atlas {

void ProjectPanel::onImGuiRender() {
	m_activeProject = ProjectManager::getActiveProject();

	ImGui::Begin("Project");
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);

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
		if (drawSceneNode(s)) {
			break;
		}
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2(100 * EditorWidgets::displayScale, 0), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::BeginPopupContextWindow("SceneHierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		if (ImGui::MenuItem("Add Existing Scene")) {
			std::string filepath = Platform::openFileDialog("atscene");
			if (!filepath.empty()) {
				auto scene = std::make_shared<Scene>(
					std::filesystem::path(filepath).stem().string());
				scene->getPath() = filepath;
				ProjectManager::attachScenetoProject(scene);
				ProjectManager::saveProject();
			}
		}
		ImGui::EndPopup();
	}
	ImGui::End();
}

// returns true if scene was removed from project
bool ProjectPanel::drawSceneNode(std::string file_path) {
	std::string displayName = std::filesystem::path(file_path).stem().string();

	bool isSelected = m_selectionContext && ProjectManager::toAbsolutePath(file_path) == m_selectionContext->getPath();

	bool wasDimmed = false;
	if (isSelected) {
		bool panelFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		if (!panelFocused) {
			wasDimmed = true;
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorWidgets::green);
			ImGui::PushStyleColor(ImGuiCol_Header, EditorWidgets::greenSub);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(EditorWidgets::greenSub.x, EditorWidgets::greenSub.y, EditorWidgets::greenSub.z, EditorWidgets::greenSub.w * 0.5f));
		}
	}

	if (ImGui::Selectable(displayName.c_str(), isSelected)) {
		auto scene		   = ProjectManager::loadScene(ProjectManager::toAbsolutePath(file_path));
		m_selectionContext = scene;
		m_onSceneSelected(scene);
	}

	bool removeFromProject = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Set as Startup Scene")) {
			ProjectManager::getActiveProject()->getData().startup_scene = file_path;
			ProjectManager::saveProject();
		}
		if (ImGui::MenuItem("Remove from Project")) {
			removeFromProject = true;
		}
		ImGui::EndPopup();
	}

	if (file_path == ProjectManager::getActiveProject()->getData().startup_scene) {
		float  radius	   = ImGui::GetTextLineHeight() * 0.3f;
		float  padding	   = 4.0f;
		ImVec2 windowPos   = ImGui::GetWindowPos();
		float  windowWidth = ImGui::GetWindowSize().x;
		float  scrollX	   = ImGui::GetScrollX();
		float  dotX		   = windowPos.x + windowWidth - radius * 2.0f - padding - scrollX;
		float  dotY		   = ImGui::GetCursorScreenPos().y - ImGui::GetTextLineHeight() * 0.5f;

		ImGui::GetWindowDrawList()->AddCircleFilled(
			ImVec2(dotX, dotY),
			radius,
			ImGui::ColorConvertFloat4ToU32(EditorWidgets::green));
	}

	if (wasDimmed) ImGui::PopStyleColor(3);

	if (removeFromProject) {
		auto& paths = ProjectManager::getActiveProject()->getData().scene_filepaths;
		paths.erase(std::remove(paths.begin(), paths.end(), file_path), paths.end());
		if (ProjectManager::getActiveProject()->getData().startup_scene == file_path)
			ProjectManager::getActiveProject()->getData().startup_scene = "";
		ProjectManager::saveProject();
		return true;
	}
	return false;
}

}  // namespace Atlas