#pragma once

#include <memory>
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Registry.h"
#include "Atlas/Renderer/TextureSheet.h"
#include "Atlas/Scene/Scene.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Atlas {

class SceneHierarchyPanel {
  public:
	SceneHierarchyPanel(std::shared_ptr<Scene> scene) : m_scene(scene) {}
	~SceneHierarchyPanel() = default;

	void setScene(std::shared_ptr<Scene> scene) {
		m_scene			   = scene;
		m_selectionContext = {};
		m_renameTarget	   = {};
	}
	void addEmptyEntity() {
		m_renameTarget		= m_scene->createEntity("New Entity");
		m_focusRenameCursor = true;
	}

	void		  addComponent() { m_openComponentPicker = true; }
	const Entity& getSelectionContext() const { return m_selectionContext; }

	void onImGuiRender() {
		if (!m_scene) {
			return;
		}

		ImGui::Begin("Scene Hierarchy");

		ImGui::Text("%s", m_scene->getName().c_str());

		if (ImGui::Button("Add Entity")) {
			addEmptyEntity();
		}

		Registry& registry = m_scene->getRegistry();
		auto	  view	   = registry.view<Component::Tag>();
		for (entt::entity entity : view) {
			Entity e{entity, m_scene.get()};
			ImGui::PushID(e.getComponent<Component::UUID>().id);
			drawEntityNode(e);
			ImGui::PopID();
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(360, 0), ImVec2(FLT_MAX, FLT_MAX));
		if (ImGui::BeginPopupContextWindow("SceneHierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Add entity", "Cmd+E")) {
				addEmptyEntity();
			}
			ImGui::EndPopup();
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

		ImGui::Begin("Properties");
		if (m_selectionContext) {
			drawComponents(m_selectionContext);
		}
		ImGui::End();
	}

  private:
	void drawEntityNode(Entity& entity) {
		auto&			   tag		  = entity.getComponent<Component::Tag>();
		ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		bool opened = false;
		if (entity == m_renameTarget) {
			if (m_focusRenameCursor) {
				ImGui::SetKeyboardFocusHere();
				m_focusRenameCursor = false;
			}
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::InputText("##rename", &tag.tag, ImGuiInputTextFlags_AutoSelectAll);
		} else {
			opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.getUUID(), tree_flags, "%s", tag.tag.c_str());
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			m_renameTarget		= entity;
			m_focusRenameCursor = true;
		}

		if (ImGui::IsItemClicked()) {
			m_selectionContext = entity;
		}

		if (entity == m_renameTarget && ImGui::IsItemDeactivated()) {
			m_renameTarget = {};
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Rename")) {
				m_renameTarget = entity;
				m_focusRenameCursor = true;
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
			m_scene->getRegistry().destroy(entity);
			if (m_selectionContext == entity) {
				m_selectionContext = {};
			}
		}
	}

	template <typename T, typename UIFunction>
	void drawComponent(const char* label, Entity& entity, UIFunction uiFunction) {
		if (!entity.hasComponent<T>())
			return;

		auto&					 component	   = entity.getComponent<T>();
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		// TODO: Update formatting
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", label);
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		std::string popupID = std::string("ComponentSettings##") + typeid(T).name();
		if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
			ImGui::OpenPopup(popupID.c_str());
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup(popupID.c_str())) {
			if (ImGui::MenuItem("Delete component"))
				removeComponent = true;
			ImGui::EndPopup();
		}

		if (open) {
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			entity.removeComponent<T>();
		}
	}

	void drawComponents(Entity& entity) {
		auto& UUID = entity.getComponent<Component::UUID>().id;
		ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

		drawComponent<Component::Transform>("Transform", entity, [this](auto& component) {
			drawVec3Control("Position", component.position, 0.0f, 120.0f);
		});
		drawComponent<Component::Sprite>("Sprite", entity, [this](auto& component) {
			SubTextureSpecification& specs	  = component.subtexture->getSpecs();
			std::string&			 filepath = component.subtexture->getTexture()->getFilepath();
			ImGui::Text("%s", filepath.c_str());
			ImGui::Text("Tile size: %.f x %.f", specs.tileDims.x, specs.tileDims.y);
			ImGui::Text("[%d] [%d]", specs.index.y, specs.index.x);
		});
	}

	void drawVec3Control(const char* label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		ImGuiIO io		 = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float  lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void drawComponentPicker(Entity& entity) {
		if (ImGui::MenuItem("Animation")) {
			AT_CORE_WARN("Cannot add Animation component: Default constructor isn't implemented yet");
		}
		if (ImGui::MenuItem("Script")) {
			AT_CORE_WARN("Cannot add Script component: Default constructor isn't implemented yet");
		}
	}

  private:
	std::shared_ptr<Scene> m_scene;

	Entity m_selectionContext;
	Entity m_renameTarget;
	bool   m_openComponentPicker = false;
	bool   m_focusRenameCursor	 = false;
};

}  // namespace Atlas