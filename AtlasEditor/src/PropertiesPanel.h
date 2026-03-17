#pragma once

#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ImGui/ImGuiSystem.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas {

class PropertiesPanel {
  public:
	PropertiesPanel(std::shared_ptr<Scene> scene) : m_scene(scene) {}
	~PropertiesPanel() = default;

	void onImGuiRender(Entity& selection);
	void setScene(std::shared_ptr<Scene> scene) { m_scene = scene; }

  private:
	enum class SelectionType { None,
							   AnimationClip,
							   AudioClip };

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

	void drawComponents(Entity& entity);

	void drawClipLabel(std::string& clip, SelectionType type);

	bool drawVec3Control(const char* label, glm::vec3& values, float resetX = 0.0f, float resetY = 0.0f, float resetZ = 0.0f, float columnWidth = 100.0f);

	template <typename T>
	bool drawVec2Control(const char* label, T& values, typename T::value_type resetX = 0, typename T::value_type resetY = 0, float columnWidth = 100.0f) {
		ImGuiIO io		 = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float  lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiSystem::steelBlue);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiSystem::steelBlueLight);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiSystem::steelBlueActive);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) values.x = resetX;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if constexpr (std::is_same_v<T, glm::ivec2>) {
			changed |= ImGui::DragInt("##X", &values.x, 1, 0, 0, "%d");
		} else {
			changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImGuiSystem::green);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiSystem::greenLight);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiSystem::greenActive);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) values.y = resetY;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if constexpr (std::is_same_v<T, glm::ivec2>) {
			changed |= ImGui::DragInt("##Y", &values.y, 1, 0, 0, "%d");
		} else {
			changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

  private:
	std::shared_ptr<Scene> m_scene;

	SelectionType m_selectionType = PropertiesPanel::SelectionType::None;
	std::string	  m_selectedClip;

	std::string m_clipToDelete;

	std::string m_renameTarget;
	std::string m_renamedClip;
	std::string m_renameBuffer;
	int			m_focusRenameCursor	 = 0;
	bool		m_justFinishedRename = false;
};

}  // namespace Atlas