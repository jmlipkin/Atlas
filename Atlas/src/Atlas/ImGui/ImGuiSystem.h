#pragma once

#include <imgui/imgui.h>
#include <memory>

namespace Atlas {

class ImGuiSystem {
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
           IM_COL32( 62, 201, 106, 220),   // Green      #3EC96A
           IM_COL32(185, 138, 232, 220),   // Purple     #B98AE8
           IM_COL32(106, 172, 236, 210),   // Steel blue #6AACEC
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