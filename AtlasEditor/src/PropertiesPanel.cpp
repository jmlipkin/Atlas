#include "atpch.h"
#include "PropertiesPanel.h"

#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"
#include "Atlas/ImGui/ImGuiSystem.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Animation.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas {

void PropertiesPanel::onImGuiRender(Entity& selection) {
	ImGui::Begin("Properties");
	ImGuiSystem::DrawPanelAccentBar(ImGuiSystem::PanelAccent::Purple);

	if (selection)
		drawComponents(selection);

	ImGui::End();
}

void PropertiesPanel::drawComponents(Entity& entity) {
	auto& UUID = entity.getComponent<Component::UUID>().id;
	ImGui::Text("UUID: 0x%016llX", (uint64_t)UUID);

	drawComponent<Component::Transform>("Transform", entity, [this](auto& component) {
		drawVec3Control("Position", component.position, 0.0f, 0.0f, 1.0f, 120.0f);
	});
	drawComponent<Component::Sprite>("Sprite", entity, [this](auto& component) {
		SubTextureSpecification& specs	  = component.specs;
		const std::string&		 filepath = ProjectManager::toRelativePath(component.texturePath);

		ImGui::Text("%s", filepath.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Change")) {
			component.texturePath = Platform::openFileDialog("png");
			ProjectManager::saveScene(m_scene);
		}

		bool changed = false;
		changed |= drawVec2Control<glm::vec2>("Size (tiles)", specs.sizeInTiles);
		changed |= drawVec2Control<glm::vec2>("Grid Size", specs.tileSize);
		
		changed |= drawVec2Control<glm::ivec2>("Index", specs.index);

		if(changed) {
			component.recalculateCoordinates();
			ProjectManager::saveScene(m_scene);
		}
	});
	drawComponent<Component::Animations>("Animations", entity, [this](auto& component) {

	});
}

bool PropertiesPanel::drawVec3Control(const char* label, glm::vec3& values, float resetX, float resetY, float resetZ, float columnWidth) {
	ImGuiIO io		 = ImGui::GetIO();
	ImFont* boldFont = io.Fonts->Fonts[0];

	// Theme colors
	static const ImVec4 xColor		 = ImVec4(0.416f, 0.675f, 0.925f, 1.00f);  // Steel Blue
	static const ImVec4 xColorHover	 = ImVec4(0.516f, 0.775f, 1.000f, 1.00f);
	static const ImVec4 xColorActive = ImVec4(0.278f, 0.518f, 0.780f, 1.00f);
	static const ImVec4 yColor		 = ImVec4(0.243f, 0.788f, 0.416f, 1.00f);  // Green
	static const ImVec4 yColorHover	 = ImVec4(0.343f, 0.888f, 0.516f, 1.00f);
	static const ImVec4 yColorActive = ImVec4(0.169f, 0.588f, 0.302f, 1.00f);
	static const ImVec4 zColor		 = ImVec4(0.725f, 0.541f, 0.910f, 1.00f);  // Purple
	static const ImVec4 zColorHover	 = ImVec4(0.825f, 0.641f, 1.000f, 1.00f);
	static const ImVec4 zColorActive = ImVec4(0.545f, 0.380f, 0.722f, 1.00f);

	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
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
	changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
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
	changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, zColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, zColorHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, zColorActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) values.z = resetZ;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();

	return changed;
}

}  // namespace Atlas