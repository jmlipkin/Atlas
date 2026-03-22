#include "atpch.h"
#include "imgui/imgui.h"
#include "EditorWidgets.h"

namespace Atlas {

float EditorWidgets::displayScale = 1.0f;

bool EditorWidgets::drawVec3Control(const char* label, glm::vec3& values, float resetX, float resetY, float resetZ, float columnWidth, float valueWidth, bool vertical) {
	ImGuiIO io		 = ImGui::GetIO();
	ImFont* boldFont = io.Fonts->Fonts[0];

	bool changed = false;

	ImGui::PushID(label);

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	float lineHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = {lineHeight, lineHeight};

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::steelBlue);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::steelBlueLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::steelBlueActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) values.x = resetX;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::PushItemWidth(valueWidth);
	changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	if (!vertical)
		ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::green);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::greenLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::greenActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize)) values.y = resetY;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::PushItemWidth(valueWidth);
	changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	if (!vertical)
		ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::purple);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::purpleLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::purpleActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) values.z = resetZ;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::PushItemWidth(valueWidth);
	changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();

	return changed;
}

void EditorWidgets::DrawPanelAccentBar(PanelAccent color, bool onlyWhenFocused) {
	if (onlyWhenFocused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
		return;

	ImDrawList* dl	= ImGui::GetWindowDrawList();
	ImVec2		pos = ImGui::GetWindowPos();
	ImVec2		sz	= ImGui::GetWindowSize();
	dl->AddRectFilled(
		ImVec2(pos.x, pos.y),
		ImVec2(pos.x + 1.5f * displayScale, pos.y + sz.y),
		getAccentColor(color));
}

ImU32 EditorWidgets::getAccentColor(PanelAccent color) {
	switch (color) {
		case PanelAccent::Blue:
			return ImGui::ColorConvertFloat4ToU32(steelBluePanelAccent);
		case PanelAccent::Green:
			return ImGui::ColorConvertFloat4ToU32(greenPanelAccent);
		case PanelAccent::Purple:
			return ImGui::ColorConvertFloat4ToU32(purplePanelAccent);
	}
}

}  // namespace Atlas