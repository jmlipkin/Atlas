#include "atpch.h"

#include "EditorLayer.h"
#include "SandboxScene.h"
#include "SceneHierarchyPanel.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Time.h"

#include "Atlas/Project/Serializer.h"

#include <imgui/imgui.h>

namespace Atlas {

EditorLayer::EditorLayer() : Layer("Editor"), m_cameraController((float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight()) {
	// TODO: Change to a more robust solution
	m_scenes.push_back(std::make_shared<SandboxScene>(m_cameraController));
	m_hierarchyPanel = new SceneHierarchyPanel(m_scenes[0]);

	Serializer::deserializeScene(m_scenes[0]);
}

void EditorLayer::onUpdate(DeltaTime dt) {
	for (auto scene : m_scenes)
		scene->onUpdate(dt);
}

void EditorLayer::onEvent(Event& event) {
	for (auto scene : m_scenes)
		scene->onEvent(event);
}

void EditorLayer::onImGuiRender() {
	static bool dockspaceOpen = true;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dockspace", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0, 0), dockspace_flags);

#ifndef AT_PLATFORM_MACOS
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
				Application::get().close();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
#endif

	m_logger.onImGuiRender();
	m_hierarchyPanel->onImGuiRender();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_viewportSize != *((glm::vec2*)&viewportSize)) {
		Application::get().getFramebuffer()->onResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		m_viewportSize = {viewportSize.x, viewportSize.y};
		m_cameraController.onResize(viewportSize.x, viewportSize.y);
	}
	void* texture = Application::get().getFramebuffer()->getColorTexture(0);
	ImGui::Image(texture, viewportSize);
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

}  // namespace Atlas