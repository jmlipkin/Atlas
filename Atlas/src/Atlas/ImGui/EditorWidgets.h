#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/glm.hpp>

namespace Atlas {

class EditorWidgets {
  public:
	constexpr static const ImVec4 bg0 = ImVec4(0.114f, 0.122f, 0.141f, 1.00f);	// #1D1F24  deepest chrome
	constexpr static const ImVec4 bg1 = ImVec4(0.145f, 0.155f, 0.180f, 1.00f);	// #25272E  main window
	constexpr static const ImVec4 bg2 = ImVec4(0.178f, 0.190f, 0.220f, 1.00f);	// #2D3038  child panels
	constexpr static const ImVec4 bg3 = ImVec4(0.212f, 0.226f, 0.261f, 1.00f);	// #363943  widget fills
	constexpr static const ImVec4 bg4 = ImVec4(0.250f, 0.265f, 0.306f, 1.00f);	// #40434E  elevated / hovered

	// --- Borders ---
	constexpr static const ImVec4 border	= ImVec4(0.318f, 0.333f, 0.380f, 1.00f);
	constexpr static const ImVec4 borderDim = ImVec4(0.245f, 0.257f, 0.294f, 0.70f);

	// --- Text ---
	constexpr static const ImVec4 textPrimary  = ImVec4(0.906f, 0.910f, 0.925f, 1.00f);
	constexpr static const ImVec4 textDisabled = ImVec4(0.370f, 0.382f, 0.415f, 1.00f);

	// --- STEEL BLUE #6AACEC — active selection & UI confirmation ---
	constexpr static const ImVec4 steelBlue			   = ImVec4(0.416f, 0.675f, 0.925f, 1.00f);	 // #6AACEC
	constexpr static const ImVec4 steelBluePanelAccent = ImVec4(0.416f, 0.675f, 0.925f, 0.824);	 // #6AACEC
	constexpr static const ImVec4 steelBlueHover	   = ImVec4(0.416f, 0.675f, 0.925f, 0.32f);
	constexpr static const ImVec4 steelBlueSub		   = ImVec4(0.416f, 0.675f, 0.925f, 0.16f);
	constexpr static const ImVec4 steelBlueActive	   = ImVec4(0.278f, 0.518f, 0.780f, 1.00f);	 // #4784C7
	constexpr static const ImVec4 steelBlueLight	   = ImVec4(0.516f, 0.775f, 1.000f, 1.00f);

	// --- GREEN #3EC96A — entity & component identity ---
	constexpr static const ImVec4 green			   = ImVec4(0.243f, 0.788f, 0.416f, 1.00f);	 // #3EC96A
	constexpr static const ImVec4 greenPanelAccent = ImVec4(0.243f, 0.788f, 0.416f, 0.863f);
	constexpr static const ImVec4 greenHover	   = ImVec4(0.243f, 0.788f, 0.416f, 0.32f);
	constexpr static const ImVec4 greenSub		   = ImVec4(0.243f, 0.788f, 0.416f, 0.14f);	 // resting entity row tint
	constexpr static const ImVec4 greenActive	   = ImVec4(0.169f, 0.588f, 0.302f, 1.00f);	 // #2B964D confirmed pick
	constexpr static const ImVec4 greenLight	   = ImVec4(0.343f, 0.888f, 0.516f, 1.00f);

	// --- PURPLE #B98AE8 — panel focus & chrome ---
	constexpr static const ImVec4 purple			= ImVec4(0.725f, 0.541f, 0.910f, 1.00f);  // #B98AE8
	constexpr static const ImVec4 purplePanelAccent = ImVec4(0.725f, 0.541f, 0.910f, 0.863f);
	constexpr static const ImVec4 purpleHover		= ImVec4(0.725f, 0.541f, 0.910f, 0.32f);
	constexpr static const ImVec4 purpleActive		= ImVec4(0.545f, 0.380f, 0.722f, 1.00f);  // #8B61B8
	constexpr static const ImVec4 purpleTabBg		= ImVec4(0.228f, 0.196f, 0.318f, 1.00f);  // #3A3251 active tab
	constexpr static const ImVec4 purpleLight		= ImVec4(0.825f, 0.641f, 1.000f, 1.00f);

	// --- TEAL #3BBFA8 — plot lines & data vis ---
	constexpr static const ImVec4 teal = ImVec4(0.231f, 0.749f, 0.659f, 1.00f);	 // #3BBFA8

	enum class PanelAccent { Green,
							 Purple,
							 Blue };

	static float displayScale;

	//  Usage:
	//    if (ImGui::Begin("Scene Hierarchy"))
	//    {
	//        DrawPanelAccentBar(PanelAccent::Green);
	//        // ... content ...
	//    }
	//    ImGui::End();
	//
	//    if (ImGui::Begin("Inspector"))
	//    {
	//        DrawPanelAccentBar(PanelAccent::Purple);
	//        // ... content ...
	//    }
	//    ImGui::End();
	static void DrawPanelAccentBar(PanelAccent color, bool onlyWhenFocused = true);

	static void drawImageWithAspectRatio(void* data, float aspectRatio, bool centered = false);

	static bool drawFloatSlider(const char* label, float& value, float columnWidth = 85.0f, float valueWidth = 120.0f, float resetValue = 0.0f, float min = 0.0f, float max = 1.0f, float speed = 0.1f);

	static void drawLabel(const char* label, float columnWidth);
	static bool drawCheckbox(const char* label, bool& value, float columnWidth = 85.0f);
	static bool drawIntControl(const char* label, int& value, const char* resetLabel = "R", float columnWidth = 85.0f, float valueWidth = 120.0f, int resetValue = 0);
	static bool drawFloatControl(const char* label, float& value, const char* resetLabel = "R", float columnWidth = 85.0f, float valueWidth = 100.0f, float resetValue = 0.0f, float speed = 0.1f);
	static bool drawCombo(const char* label, const char** items, int count, int& value, float columnWidth = 85.0f, float valueWidth = 120.0f);
	static bool drawVec3Control(const char* label, glm::vec3& values, float columnWidth = 100.0f, float valueWidth = 100.0f, float resetX = 0.f, float resetY = 0.0f, float resetZ = 0.0f, bool vertical = false);

	template <typename T>
	static bool drawVec2Control(const char* label, T& values, float columnWidth = 100.0f, float valueWidth = 100.0f, typename T::value_type resetX = 0, typename T::value_type resetY = 0, bool vertical = false) {
		bool  changed	 = false;
		float buttonSize = ImGui::GetFrameHeight();
		float spacing	 = ImGui::GetStyle().ItemInnerSpacing.x;
		float pairWidth	 = buttonSize + (valueWidth);
		float totalWidth = vertical ? pairWidth : (pairWidth * 2.0f + spacing);

		ImGui::PushID(label);
		if (ImGui::BeginTable(label, 2, ImGuiTableFlags_BordersInnerV)) {
			ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, totalWidth);
			ImGui::TableNextRow();
			drawLabel(label, columnWidth);
			float lineHeight = ImGui::GetFrameHeight();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
			ImGui::PushStyleColor(ImGuiCol_Button, steelBlue);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, steelBlueLight);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, steelBlueActive);
			if (ImGui::Button("X", ImVec2(lineHeight, lineHeight))) {
				values.x = resetX;
				changed	 = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(valueWidth - lineHeight);
			if constexpr (std::is_same_v<T, glm::ivec2>) {
				changed |= ImGui::DragInt("##X", &values.x, 1, 0, 0, "%d");
			} else {
				changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			}

			if (vertical) {
				ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
			} else {
				ImGui::SameLine();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, green);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, greenLight);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, greenActive);
			if (ImGui::Button("Y", ImVec2(lineHeight, lineHeight))) {
				values.y = resetY;
				changed	 = true;
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(valueWidth - lineHeight);
			if constexpr (std::is_same_v<T, glm::ivec2>) {
				changed |= ImGui::DragInt("##Y", &values.y, 1, 0, 0, "%d");
			} else {
				changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			}

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}
		ImGui::PopID();
		return changed;
	}

  private:
	static ImU32 getAccentColor(PanelAccent color);
};

}  // namespace Atlas