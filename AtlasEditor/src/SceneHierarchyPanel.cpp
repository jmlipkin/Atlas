#include "atpch.h"
#include "SceneHierarchyPanel.h"

#include "Commands/EntityCommands.h"
#include "Commands/ComponentCommands.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/EditorWidgets.h"
#include "Atlas/Renderer/SubTexture.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Components/Collider.h"

namespace Atlas {

void SceneHierarchyPanel::setScene(std::shared_ptr<Scene> scene) {
	m_scene			   = scene;
	m_selectionContext = {};
	m_renameTarget	   = {};
	m_renameBuffer	   = {};
	m_propertiesPanel.setScene(scene);
}

void SceneHierarchyPanel::addEmptyEntity() {
	std::unique_ptr<CreateEntityCommand> cmd = std::make_unique<CreateEntityCommand>(m_scene, "New Entity");
	CreateEntityCommand*				 raw = cmd.get();
	m_commandHistory.push(std::move(cmd));
	autoSave();

	m_selectionContext	= raw->getEntity();
	m_renameTarget		= raw->getEntity();
	m_renameBuffer		= raw->getEntity().name();
	m_focusRenameCursor = 2;
}

void SceneHierarchyPanel::onImGuiRender() {
	if (!m_scene) {
		return;
	}

	ImGui::Begin("Scene Hierarchy");
	EditorWidgets::DrawPanelAccentBar(EditorWidgets::PanelAccent::Purple);

	ImGui::Text("%s", m_scene->getName().c_str());

	if (ImGui::Button("Add Entity")) {
		addEmptyEntity();
	}

	Registry& registry = m_scene->getRegistry();
	auto	  view	   = registry.view<Component::Tag>();
	for (entt::entity entity : view) {
		Entity	 e{entity, m_scene.get()};
		uint64_t uuid = e.getUUID();
		ImGui::PushID(&uuid);
		drawEntityNode(e);
		ImGui::PopID();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(0) && !ImGui::IsAnyItemHovered()) {
		if (!m_renameTarget)  // Don't deselect while renaming
			m_selectionContext = {};
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2(100 * EditorWidgets::displayScale, 0), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::BeginPopupContextWindow("SceneHierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		if (ImGui::MenuItem("Add entity", "Cmd+E")) {
			addEmptyEntity();
		}
		ImGui::EndPopup();
	}

	// -1 means we just finished renaming, and so we need to skip this frame
	if (m_justFinishedRename) {
		m_justFinishedRename = false;
	} else if (ImGui::IsWindowFocused() && m_selectionContext && !m_renameTarget && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		m_renameTarget		= m_selectionContext;
		m_renameBuffer		= m_selectionContext.name();
		m_focusRenameCursor = 2;
	}

	if (m_openComponentPicker) {
		ImGui::OpenPopup("AddComponentPopup");
		m_openComponentPicker = false;
	}
	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (m_selectionContext)
			drawComponentPicker(m_selectionContext);
		ImGui::EndPopup();
	}

	ImGui::End();

	m_propertiesPanel.onImGuiRender(m_selectionContext);
}

void SceneHierarchyPanel::drawEntityNode(Entity& entity) {
	auto&			   tag		  = entity.getComponent<Component::Tag>();
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	bool wasDimmed = false;
	if (entity == m_selectionContext) {
		tree_flags |= ImGuiTreeNodeFlags_Selected;

		bool panelFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		if (!panelFocused) {
			wasDimmed = true;
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, EditorWidgets::green);
			ImGui::PushStyleColor(ImGuiCol_Header, EditorWidgets::greenSub);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(EditorWidgets::greenSub.x, EditorWidgets::greenSub.y, EditorWidgets::greenSub.z, EditorWidgets::greenSub.w * 0.5f));
		}
	}

	bool opened = false;
	if (entity == m_renameTarget) {
		if (m_focusRenameCursor > 0) {
			ImGui::SetKeyboardFocusHere();
			m_focusRenameCursor--;
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##rename", &m_renameBuffer, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
			m_commandHistory.push(std::make_unique<RenameEntityCommand>(entity, tag.tag, m_renameBuffer));
			m_renameTarget = {};
			m_renameBuffer = {};
			ImGui::SetKeyboardFocusHere(-1);
			m_justFinishedRename = true;
			autoSave();
		}
	} else {
		opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.getUUID(), tree_flags, "%s", tag.tag.c_str());
	}

	if (wasDimmed) ImGui::PopStyleColor(3);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		m_renameTarget		= entity;
		m_renameBuffer		= tag.tag;
		m_focusRenameCursor = 2;
	}

	if (ImGui::IsItemClicked()) {
		m_selectionContext = entity;
	}

	if (entity == m_renameTarget && ImGui::IsItemDeactivated()) {
		m_renameTarget = {};
		m_renameBuffer = {};
		autoSave();
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Rename")) {
			m_renameTarget		= entity;
			m_renameBuffer		= tag.tag;
			m_focusRenameCursor = 2;
		}
		if (ImGui::BeginMenu("Add Component")) {
			drawComponentPicker(entity);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Delete"))
			entityDeleted = true;
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (entityDeleted) {
		if (m_selectionContext == entity) {
			m_selectionContext = {};
		}
		m_commandHistory.push(std::make_unique<DeleteEntityCommand>(m_scene, entity));
		autoSave();
	}
}

void SceneHierarchyPanel::drawComponentPicker(Entity& entity) {
	if (ImGui::MenuItem("Animation")) {
		if (!entity.hasComponent<Component::Animations>()) {
			m_commandHistory.push(makeAddComponentCommand<Component::Animations>(entity));
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has Animations component!", entity.name());
		}
	}

	if (ImGui::MenuItem("Collider")) {
		if (!entity.hasComponent<Component::Collider>()) {
			m_commandHistory.push(makeAddComponentCommand<Component::Collider>(entity));
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has Collider component!", entity.name());
		}
	}

	if (ImGui::MenuItem("Sprite")) {
		if (!entity.hasComponent<Component::Sprite>()) {
			std::string filepath = Platform::openFileDialog("png");
			m_commandHistory.push(makeAddComponentCommand<Component::Sprite>(entity, filepath, SubTextureSpecification{}));
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has Sprite component!", entity.name());
		}
	}

	if (ImGui::MenuItem("RigidBody")) {
		if (!entity.hasComponent<Component::RigidBody>()) {
			m_commandHistory.push(makeAddComponentCommand<Component::RigidBody>(entity));
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has RigidBody component!", entity.name());
		}
	}

	if (ImGui::MenuItem("Script")) {
		if (!entity.hasComponent<Component::Script>()) {
			m_commandHistory.push(makeAddComponentCommand<Component::Script>(entity));
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has Script component!", entity.name());
		}
	}

	if (ImGui::MenuItem("Tile Map")) {
		if (!entity.hasComponent<Component::Tilemap>()) {
			m_commandHistory.push(makeAddComponentCommand<Component::Tilemap>(entity));
			m_propertiesPanel.setTilemapOpenState(true);
			autoSave();
		} else {
			AT_CORE_WARN("Entity \"{}\" already has Tilemap component!", entity.name());
		}
	}
}

void SceneHierarchyPanel::autoSave() {
	if (std::filesystem::path(m_scene->getPath()).is_absolute()) {
		ProjectManager::saveScene(m_scene);
	}
}

}  // namespace Atlas