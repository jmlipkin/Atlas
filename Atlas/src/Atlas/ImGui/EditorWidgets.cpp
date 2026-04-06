#include "atpch.h"
#include "imgui/imgui.h"
#include "EditorWidgets.h"

namespace Atlas {

float EditorWidgets::displayScale = 1.0f;

void EditorWidgets::drawImageWithAspectRatio(void* data, float aspectRatio, bool centered) {
	ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

	float contentRegionAspectRatio = contentRegionAvail.x / contentRegionAvail.y;

	ImVec2 finalSize;
	if (contentRegionAspectRatio > aspectRatio) {
		finalSize.y = contentRegionAvail.y;
		finalSize.x = finalSize.y * aspectRatio;
	} else {
		finalSize.x = contentRegionAvail.x;
		finalSize.y = finalSize.x / aspectRatio;
	}

	if (centered) {
		ImVec2 cursor_pos = ImGui::GetCursorPos();
		ImVec2 padding;
		padding.x = (contentRegionAvail.x - finalSize.x) * 0.5f;
		padding.y = (contentRegionAvail.y - finalSize.y) * 0.5f;
		ImGui::SetCursorPos(ImVec2(cursor_pos.x + padding.x, cursor_pos.y + padding.y));
	}

	ImGui::Image(data, finalSize);
}

bool EditorWidgets::drawFloatSlider(const char* label, float& value, float columnWidth, float valueWidth, float resetValue, float min, float max, float speed) {
	bool changed = false;
	ImGui::PushID(label);
	bool			hasLabel = label[0] != '\0';
	ImGuiTableFlags flags	 = hasLabel ? ImGuiTableFlags_BordersInnerV : 0;
	int				columns	 = hasLabel ? 2 : 1;
	if (ImGui::BeginTable(label, columns, flags)) {
		if (hasLabel) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		} else {
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		}
		ImGui::TableNextRow();
		if (hasLabel) {
			drawLabel(label, columnWidth);

		} else {
			ImGui::TableSetColumnIndex(0);
		}
		float lineHeight = ImGui::GetFrameHeight();

		ImVec2 sliderMin   = ImGui::GetCursorScreenPos();
		float  sliderWidth = valueWidth;
		float  centerY	   = sliderMin.y + ImGui::GetFrameHeight() * 0.5f;

		ImGui::GetWindowDrawList()->AddLine(
			{sliderMin.x, centerY},
			{sliderMin.x + sliderWidth, centerY},
			ImGui::ColorConvertFloat4ToU32(EditorWidgets::border),
			1.0f);

		ImGui::SetNextItemWidth(-1);

		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, EditorWidgets::steelBlue);
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, EditorWidgets::steelBlueActive);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 6.0f * EditorWidgets::displayScale);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

		changed |= ImGui::SliderFloat("##v", &value, min, max, "%.f");

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(5);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			changed = true;
			value	= resetValue;
		}

		ImGui::EndTable();
	}
	ImGui::PopID();
	return changed;
}

void EditorWidgets::drawLabel(const char* label, float columnWidth) {
	ImGui::TableSetColumnIndex(0);
	float textWidth = ImGui::CalcTextSize(label).x;
	float columnEnd = ImGui::GetCursorPosX() + columnWidth - ImGui::GetStyle().ItemSpacing.x;
	ImGui::SetCursorPosX(columnEnd - textWidth);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s", label);
	ImGui::TableSetColumnIndex(1);
}

bool EditorWidgets::drawIntControl(const char* label, int& value, const char* resetLabel, float columnWidth, float valueWidth, int resetValue) {
	bool changed = false;
	ImGui::PushID(label);
	bool			hasLabel = label[0] != '\0';
	ImGuiTableFlags flags	 = hasLabel ? ImGuiTableFlags_BordersInnerV : 0;
	int				columns	 = hasLabel ? 2 : 1;
	if (ImGui::BeginTable(label, columns, flags)) {
		if (hasLabel) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		} else {
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		}
		ImGui::TableNextRow();
		if (hasLabel)
			drawLabel(label, columnWidth);
		float lineHeight = ImGui::GetFrameHeight();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
		ImGui::PushStyleColor(ImGuiCol_Button, purple);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, purpleLight);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, purpleActive);
		if (ImGui::Button(resetLabel, ImVec2(lineHeight, lineHeight))) {
			value	= resetValue;
			changed = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(valueWidth /* * displayScale */ - lineHeight);
		changed |= ImGui::DragInt("##v", &value);
		ImGui::PopStyleVar();
		ImGui::EndTable();
	}
	ImGui::PopID();
	return changed;
}

bool EditorWidgets::drawFloatControl(const char* label, float& value, const char* resetLabel, float columnWidth, float valueWidth, float resetValue, float speed) {
	bool changed = false;
	ImGui::PushID(label);
	bool			hasLabel = label[0] != '\0';
	ImGuiTableFlags flags	 = hasLabel ? ImGuiTableFlags_BordersInnerV : 0;
	int				columns	 = hasLabel ? 2 : 1;
	if (ImGui::BeginTable(label, columns, flags)) {
		if (hasLabel) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		} else {
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		}
		ImGui::TableNextRow();

		if (hasLabel)
			drawLabel(label, columnWidth);

		float lineHeight = ImGui::GetFrameHeight();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
		ImGui::PushStyleColor(ImGuiCol_Button, purple);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, purpleLight);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, purpleActive);
		if (ImGui::Button(resetLabel, ImVec2(lineHeight, lineHeight))) {
			value	= resetValue;
			changed = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-1);
		changed |= ImGui::DragFloat("##v", &value, speed, 0.0f, 0.0f, "%.2f");
		ImGui::PopStyleVar();
		ImGui::EndTable();
	}
	ImGui::PopID();
	return changed;
}

bool EditorWidgets::drawCheckbox(const char* label, bool& value, float columnWidth) {
	bool changed = false;
	ImGui::PushID(label);
	bool			hasLabel = label[0] != '\0';
	ImGuiTableFlags flags	 = hasLabel ? ImGuiTableFlags_BordersInnerV : 0;
	int				columns	 = hasLabel ? 2 : 1;

	if (ImGui::BeginTable(label, columns, flags)) {
		if (hasLabel) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
		} else {
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
		}
		ImGui::TableNextRow();
		if (hasLabel)
			drawLabel(label, columnWidth);
		changed = ImGui::Checkbox("##v", &value);
		ImGui::EndTable();
	}
	ImGui::PopID();
	return changed;
}

bool EditorWidgets::drawCombo(const char* label, const char** items, int count, int& value, float columnWidth, float valueWidth) {
	bool changed = false;
	ImGui::PushID(label);

	bool			hasLabel = label[0] != '\0';
	ImGuiTableFlags flags	 = hasLabel ? ImGuiTableFlags_BordersInnerV : 0;
	int				columns	 = hasLabel ? 2 : 1;
	if (ImGui::BeginTable(label, columns, flags)) {
		if (hasLabel) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		} else {
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
		}
		ImGui::TableNextRow();
		if (hasLabel)
			drawLabel(label, columnWidth);

		ImGui::SetNextItemWidth(-1);
		changed = ImGui::Combo("##v", &value, items, count);
		ImGui::EndTable();
	}
	ImGui::PopID();
	return changed;
}

bool EditorWidgets::drawVec3Control(const char* label, glm::vec3& values, float resetX, float resetY, float resetZ, float columnWidth, float valueWidth, bool vertical) {
	ImGuiIO io		 = ImGui::GetIO();
	ImFont* boldFont = io.Fonts->Fonts[0];

	bool changed = false;

	ImGui::PushID(label);

	float  lineHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = {lineHeight, lineHeight};

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);

	if (vertical) {
		float totalHeight = 3.0f * lineHeight + 2.0f * ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
	} else {
		float totalHeight = lineHeight + 2.0f * ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (totalHeight - lineHeight) * 0.5f);
	}
	ImGui::Text("%s", label);

	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::steelBlue);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::steelBlueLight);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorWidgets::steelBlueActive);
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) {
		values.x = resetX;
		changed	 = true;
	}
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
	if (ImGui::Button("Y", buttonSize)) {
		values.y = resetY;
		changed	 = true;
	}
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
	if (ImGui::Button("Z", buttonSize)) {
		values.z = resetZ;
		changed	 = true;
	}
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
		ImVec2(pos.x + 1.5f /* * displayScale */, pos.y + sz.y),
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