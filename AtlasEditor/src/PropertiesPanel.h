#pragma once

#include "Atlas/ECS/Entities/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas {

class PropertiesPanel {
  public:
	PropertiesPanel() {}
	~PropertiesPanel() = default;

	void onImGuiRender(Entity& selection);

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

	void drawVec3Control(const char* label, glm::vec3& values, float resetX = 0.0f, float resetY = 0.0f, float resetZ = 0.0f, float columnWidth = 100.0f);
};

}  // namespace Atlas