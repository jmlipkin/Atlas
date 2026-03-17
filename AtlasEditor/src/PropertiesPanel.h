#pragma once

#include "Atlas/ECS/Entities/Entity.h"

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

	bool drawVec3Control(const char* label, glm::vec3& values, float resetX = 0.0f, float resetY = 0.0f, float resetZ = 0.0f, float columnWidth = 100.0f);

	template <typename T>
	bool drawVec2Control(const char* label, T& values, typename T::value_type resetX = 0, typename T::value_type resetY = 0, float columnWidth = 100.0f) {
		ImGuiIO io		 = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		static const ImVec4 xColor		 = ImVec4(0.416f, 0.675f, 0.925f, 1.00f);
		static const ImVec4 xColorHover	 = ImVec4(0.516f, 0.775f, 1.000f, 1.00f);
		static const ImVec4 xColorActive = ImVec4(0.278f, 0.518f, 0.780f, 1.00f);
		static const ImVec4 yColor		 = ImVec4(0.243f, 0.788f, 0.416f, 1.00f);
		static const ImVec4 yColorHover	 = ImVec4(0.343f, 0.888f, 0.516f, 1.00f);
		static const ImVec4 yColorActive = ImVec4(0.169f, 0.588f, 0.302f, 1.00f);

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
		ImGui::PushStyleColor(ImGuiCol_Button, xColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, xColorHover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, xColorActive);
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
		ImGui::PushStyleColor(ImGuiCol_Button, yColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, yColorHover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, yColorActive);
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
};

}  // namespace Atlas