#include "atpch.h"
#include "ImGuiSystem.h"

#include "Atlas/Renderer/RendererAPI.h"
#include "Atlas/Platform/OpenGL/OpenGLImGuiSystem.h"
#include "Atlas/Platform/Metal/MetalImGuiSystem.h"

#include <imgui/imgui.h>

namespace Atlas {

std::unique_ptr<ImGuiSystem> ImGuiSystem::create() {
	switch (RendererAPI::getAPI()) {
		case RendererAPI::API::None: {
			AT_ASSERT(false, "RendererAPI::None is not supported");
			break;
		}
		case RendererAPI::API::OpenGL: {
			return std::make_unique<OpenGLImGuiSystem>();
		}
		case RendererAPI::API::Metal: {
			return std::make_unique<MetalImGuiSystem>();
		}
	}

	AT_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}

void ImGuiSystem::setupImGuiStyle(ImGuiStyle& style)
{
    // -------------------------------------------------------------------------
    //  Geometry
    // -------------------------------------------------------------------------
    style.WindowPadding            = ImVec2(10.0f, 10.0f);
    style.FramePadding             = ImVec2(6.0f,  4.0f);
    style.CellPadding              = ImVec2(6.0f,  4.0f);
    style.ItemSpacing              = ImVec2(8.0f,  5.0f);
    style.ItemInnerSpacing         = ImVec2(5.0f,  4.0f);
    style.IndentSpacing            = 18.0f;
    style.ScrollbarSize            = 11.0f;
    style.GrabMinSize              = 10.0f;
 
    style.WindowRounding           = 5.0f;
    style.ChildRounding            = 4.0f;
    style.FrameRounding            = 4.0f;
    style.PopupRounding            = 5.0f;
    style.ScrollbarRounding        = 4.0f;
    style.GrabRounding             = 3.0f;
    style.TabRounding              = 4.0f;
    style.LogSliderDeadzone        = 4.0f;
 
    style.WindowBorderSize         = 1.0f;
    style.ChildBorderSize          = 1.0f;
    style.PopupBorderSize          = 1.0f;
    style.FrameBorderSize          = 1.0f;
    style.TabBorderSize            = 1.0f;
    style.TabBarBorderSize         = 1.0f;
 
    // -------------------------------------------------------------------------
    //  Color assignments
    // -------------------------------------------------------------------------
    ImVec4* c = style.Colors;
 
    // Text
    c[ImGuiCol_Text]                 = textPrimary;
    c[ImGuiCol_TextDisabled]         = textDisabled;
    c[ImGuiCol_TextSelectedBg]       = steelBlueHover;    // STEEL BLUE — text selection
 
    // Windows & panels
    c[ImGuiCol_WindowBg]             = bg1;
    c[ImGuiCol_ChildBg]              = bg2;
    c[ImGuiCol_PopupBg]              = ImVec4(bg0.x, bg0.y, bg0.z, 0.97f);
 
    // Borders
    c[ImGuiCol_Border]               = border;
    c[ImGuiCol_BorderShadow]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
 
    // Frames
    c[ImGuiCol_FrameBg]              = bg3;
    c[ImGuiCol_FrameBgHovered]       = bg4;
    c[ImGuiCol_FrameBgActive]        = ImVec4(bg4.x + 0.02f, bg4.y + 0.025f, bg4.z + 0.03f, 1.0f);
 
    // Title bars — PURPLE tint on focused window
    c[ImGuiCol_TitleBg]              = bg0;
    c[ImGuiCol_TitleBgActive]        = ImVec4(0.196f, 0.172f, 0.270f, 1.00f); // deep purple-slate
    c[ImGuiCol_TitleBgCollapsed]     = ImVec4(bg0.x, bg0.y, bg0.z, 0.75f);
 
    // Menu bar
    c[ImGuiCol_MenuBarBg]            = bg0;
 
    // Scrollbar — STEEL BLUE on active drag
    c[ImGuiCol_ScrollbarBg]          = ImVec4(bg1.x, bg1.y, bg1.z, 0.45f);
    c[ImGuiCol_ScrollbarGrab]        = bg4;
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.298f, 0.314f, 0.357f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]  = steelBlue;
 
    // Checkmark & slider — STEEL BLUE (UI selection confirmation)
    c[ImGuiCol_CheckMark]            = steelBlue;
    c[ImGuiCol_SliderGrab]           = steelBlue;
    c[ImGuiCol_SliderGrabActive]     = steelBlueActive;
 
    // Buttons — neutral bg, STEEL BLUE press
    c[ImGuiCol_Button]               = bg3;
    c[ImGuiCol_ButtonHovered]        = bg4;
    c[ImGuiCol_ButtonActive]         = ImVec4(steelBlueActive.x, steelBlueActive.y, steelBlueActive.z, 0.90f);
 
    // Headers — GREEN resting tint (entity rows), STEEL BLUE hover (active selection)
    // The two-tone read: "this row is an entity" (green) vs "I am about to select it" (blue)
    c[ImGuiCol_Header]               = greenHover;
    c[ImGuiCol_HeaderHovered]        = greenSub;
    c[ImGuiCol_HeaderActive]         = ImVec4(greenActive.x, greenActive.y, greenActive.z, 0.65f);
 
    // Separator — STEEL BLUE on drag
    c[ImGuiCol_Separator]            = border;
    c[ImGuiCol_SeparatorHovered]     = steelBlue;
    c[ImGuiCol_SeparatorActive]      = steelBlueActive;
 
    // Resize grip — PURPLE (panel operation)
    c[ImGuiCol_ResizeGrip]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_ResizeGripHovered]    = steelBlueHover;
    c[ImGuiCol_ResizeGripActive]     = steelBlue;
 
    // Tabs — PURPLE identity
    c[ImGuiCol_Tab]                  = ImVec4(bg0.x + 0.01f, bg0.y + 0.01f, bg0.z + 0.01f, 0.92f);
    c[ImGuiCol_TabHovered]           = ImVec4(bg3.x, bg3.y + 0.01f, bg3.z + 0.03f, 1.00f);
    c[ImGuiCol_TabActive]            = purpleTabBg;
    c[ImGuiCol_TabUnfocused]         = ImVec4(bg0.x, bg0.y, bg0.z, 0.92f);
    c[ImGuiCol_TabUnfocusedActive]   = ImVec4(purpleTabBg.x - 0.03f, purpleTabBg.y - 0.02f, purpleTabBg.z - 0.03f, 0.88f);
 
    // Docking — PURPLE preview
    c[ImGuiCol_DockingPreview]       = ImVec4(purple.x, purple.y, purple.z, 0.45f);
    c[ImGuiCol_DockingEmptyBg]       = bg0;
 
    // Plot / data vis — TEAL lines, GREEN bars
    c[ImGuiCol_PlotLines]            = teal;
    c[ImGuiCol_PlotLinesHovered]     = ImVec4(teal.x + 0.05f, teal.y, teal.z - 0.05f, 1.00f);
    c[ImGuiCol_PlotHistogram]        = green;
    c[ImGuiCol_PlotHistogramHovered] = ImVec4(green.x + 0.05f, green.y - 0.05f, green.z + 0.05f, 1.00f);
 
    // Tables
    c[ImGuiCol_TableHeaderBg]        = bg0;
    c[ImGuiCol_TableBorderStrong]    = border;
    c[ImGuiCol_TableBorderLight]     = borderDim;
    c[ImGuiCol_TableRowBg]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.038f);
 
    // Drag & drop — STEEL BLUE (something in motion)
    c[ImGuiCol_DragDropTarget]       = ImVec4(steelBlue.x, steelBlue.y, steelBlue.z, 0.92f);
 
    // Nav — STEEL BLUE focus ring; PURPLE window switcher
    c[ImGuiCol_NavHighlight]          = steelBlue;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(purple.x, purple.y, purple.z, 0.80f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
 
    // Modal overlay
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.60f);
}

}  // namespace Atlas