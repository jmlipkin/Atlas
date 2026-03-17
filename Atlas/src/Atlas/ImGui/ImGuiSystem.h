#pragma once

#include <imgui/imgui.h>
#include <memory>

namespace Atlas {

class ImGuiSystem {
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
	constexpr static const ImVec4 steelBlue		  = ImVec4(0.416f, 0.675f, 0.925f, 1.00f);	// #6AACEC
	constexpr static const ImVec4 steelBlueHover  = ImVec4(0.416f, 0.675f, 0.925f, 0.32f);
	constexpr static const ImVec4 steelBlueSub	  = ImVec4(0.416f, 0.675f, 0.925f, 0.16f);
	constexpr static const ImVec4 steelBlueActive = ImVec4(0.278f, 0.518f, 0.780f, 1.00f);	// #4784C7
	constexpr static const ImVec4 steelBlueLight  = ImVec4(0.516f, 0.775f, 1.000f, 1.00f);

	// --- GREEN #3EC96A — entity & component identity ---
	constexpr static const ImVec4 green		  = ImVec4(0.243f, 0.788f, 0.416f, 1.00f);	// #3EC96A
	constexpr static const ImVec4 greenHover  = ImVec4(0.243f, 0.788f, 0.416f, 0.32f);
	constexpr static const ImVec4 greenSub	  = ImVec4(0.243f, 0.788f, 0.416f, 0.14f);	// resting entity row tint
	constexpr static const ImVec4 greenActive = ImVec4(0.169f, 0.588f, 0.302f, 1.00f);	// #2B964D confirmed pick
	constexpr static const ImVec4 greenLight  = ImVec4(0.343f, 0.888f, 0.516f, 1.00f);

	// --- PURPLE #B98AE8 — panel focus & chrome ---
	constexpr static const ImVec4 purple	   = ImVec4(0.725f, 0.541f, 0.910f, 1.00f);	 // #B98AE8
	constexpr static const ImVec4 purpleHover  = ImVec4(0.725f, 0.541f, 0.910f, 0.32f);
	constexpr static const ImVec4 purpleActive = ImVec4(0.545f, 0.380f, 0.722f, 1.00f);	 // #8B61B8
	constexpr static const ImVec4 purpleTabBg  = ImVec4(0.228f, 0.196f, 0.318f, 1.00f);	 // #3A3251 active tab
	constexpr static const ImVec4 purpleLight  = ImVec4(0.825f, 0.641f, 1.000f, 1.00f);

	// --- TEAL #3BBFA8 — plot lines & data vis ---
	constexpr static const ImVec4 teal = ImVec4(0.231f, 0.749f, 0.659f, 1.00f);	 // #3BBFA8

  public:
	virtual ~ImGuiSystem() = default;

	virtual void initImGuiLayer()  = 0;
	virtual void cleanImGuiLayer() = 0;

	static std::unique_ptr<ImGuiSystem> create();

	enum class PanelAccent { Green,
							 Purple,
							 Blue };

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
	static inline void DrawPanelAccentBar(PanelAccent color, bool onlyWhenFocused = true) {
		if (onlyWhenFocused && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
			return;

		// static const ImU32 kColors[] = {
		// 	IM_COL32(81, 175, 112, 210),   // Green  #51AF70
		// 	IM_COL32(163, 126, 215, 210),  // Purple #A37ED7
		// 	IM_COL32(84, 160, 217, 195),   // Blue   #54A0D9
		// };

		static const ImU32 kColors[] = {
			IM_COL32(62, 201, 106, 220),   // Green      #3EC96A
			IM_COL32(185, 138, 232, 220),  // Purple     #B98AE8
			IM_COL32(106, 172, 236, 210),  // Steel blue #6AACEC
		};

		ImDrawList* dl	= ImGui::GetWindowDrawList();
		ImVec2		pos = ImGui::GetWindowPos();
		ImVec2		sz	= ImGui::GetWindowSize();
		dl->AddRectFilled(
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x + 3.0f, pos.y + sz.y),
			kColors[static_cast<int>(color)]);
	}

  protected:
	void setupImGuiStyle(ImGuiStyle& style);
};
}  // namespace Atlas