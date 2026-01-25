#include "atpch.h"
#include "imgui/imgui.h"
#include "EditorLayer.h"

namespace Atlas {

void EditorLayer::onImGuiRender() {

	static bool dockspaceOpen = true;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGui::Begin("Dockspace", &dockspaceOpen, window_flags);
	ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0,0), dockspace_flags);
	ImGui::End();
}

}