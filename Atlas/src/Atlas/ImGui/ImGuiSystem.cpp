#include "atpch.h"
#include "ImGuiSystem.h"

#include "Atlas/ImGui/EditorWidgets.h"

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
    style.WindowPadding            = ImVec2(5.0f, 5.0f);
    style.FramePadding             = ImVec2(3.0f,  2.0f);
    style.CellPadding              = ImVec2(3.0f,  2.0f);
    style.ItemSpacing              = ImVec2(4.0f,  2.5f);
    style.ItemInnerSpacing         = ImVec2(2.5f,  2.0f);
    style.IndentSpacing            = 9.0f;
    style.ScrollbarSize            = 5.5f;
    style.GrabMinSize              = 5.0f;
 
    style.WindowRounding           = 2.5f;
    style.ChildRounding            = 2.0f;
    style.FrameRounding            = 2.0f;
    style.PopupRounding            = 2.5f;
    style.ScrollbarRounding        = 2.0f;
    style.GrabRounding             = 1.5f;
    style.TabRounding              = 2.0f;
    style.LogSliderDeadzone        = 2.0f;
 
    style.WindowBorderSize         = 0.5f;
    style.ChildBorderSize          = 0.5f;
    style.PopupBorderSize          = 0.5f;
    style.FrameBorderSize          = 0.5f;
    style.TabBorderSize            = 0.5f;
    style.TabBarBorderSize         = 0.5f;
 
    // -------------------------------------------------------------------------
    //  Color assignments
    // -------------------------------------------------------------------------
    ImVec4* c = style.Colors;
 
    // Text
    c[ImGuiCol_Text]                 = EditorWidgets::textPrimary;
    c[ImGuiCol_TextDisabled]         = EditorWidgets::textDisabled;
    c[ImGuiCol_TextSelectedBg]       = EditorWidgets::steelBlueHover;    // STEEL BLUE — text selection
 
    // Windows & panels
    c[ImGuiCol_WindowBg]             = EditorWidgets::bg1;
    c[ImGuiCol_ChildBg]              = EditorWidgets::bg2;
    c[ImGuiCol_PopupBg]              = ImVec4(EditorWidgets::bg0.x, EditorWidgets::bg0.y, EditorWidgets::bg0.z, 0.97f);
 
    // Borders
    c[ImGuiCol_Border]               = EditorWidgets::border;
    c[ImGuiCol_BorderShadow]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
 
    // Frames
    c[ImGuiCol_FrameBg]              = EditorWidgets::bg3;
    c[ImGuiCol_FrameBgHovered]       = EditorWidgets::bg4;
    c[ImGuiCol_FrameBgActive]        = ImVec4(EditorWidgets::bg4.x + 0.02f, EditorWidgets::bg4.y + 0.025f, EditorWidgets::bg4.z + 0.03f, 1.0f);
 
    // Title bars — PURPLE tint on focused window
    c[ImGuiCol_TitleBg]              = EditorWidgets::bg0;
    c[ImGuiCol_TitleBgActive]        = ImVec4(0.196f, 0.172f, 0.270f, 1.00f); // deep purple-slate
    c[ImGuiCol_TitleBgCollapsed]     = ImVec4(EditorWidgets::bg0.x, EditorWidgets::bg0.y, EditorWidgets::bg0.z, 0.75f);
 
    // Menu bar
    c[ImGuiCol_MenuBarBg]            = EditorWidgets::bg0;
 
    // Scrollbar — STEEL BLUE on active drag
    c[ImGuiCol_ScrollbarBg]          = ImVec4(EditorWidgets::bg1.x, EditorWidgets::bg1.y, EditorWidgets::bg1.z, 0.45f);
    c[ImGuiCol_ScrollbarGrab]        = EditorWidgets::bg4;
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.298f, 0.314f, 0.357f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]  = EditorWidgets::steelBlue;
 
    // Checkmark & slider — STEEL BLUE (UI selection confirmation)
    c[ImGuiCol_CheckMark]            = EditorWidgets::steelBlue;
    c[ImGuiCol_SliderGrab]           = EditorWidgets::steelBlue;
    c[ImGuiCol_SliderGrabActive]     = EditorWidgets::steelBlueActive;
 
    // Buttons — neutral bg, STEEL BLUE press
    c[ImGuiCol_Button]               = EditorWidgets::bg3;
    c[ImGuiCol_ButtonHovered]        = EditorWidgets::bg4;
    c[ImGuiCol_ButtonActive]         = ImVec4(EditorWidgets::steelBlueActive.x, EditorWidgets::steelBlueActive.y, EditorWidgets::steelBlueActive.z, 0.90f);
 
    // Headers — GREEN resting tint (entity rows), STEEL BLUE hover (active selection)
    // The two-tone read: "this row is an entity" (green) vs "I am about to select it" (blue)
    c[ImGuiCol_Header]               = EditorWidgets::greenHover;
    c[ImGuiCol_HeaderHovered]        = EditorWidgets::greenSub;
    c[ImGuiCol_HeaderActive]         = ImVec4(EditorWidgets::greenActive.x, EditorWidgets::greenActive.y, EditorWidgets::greenActive.z, 0.65f);
 
    // Separator — STEEL BLUE on drag
    c[ImGuiCol_Separator]            = EditorWidgets::border;
    c[ImGuiCol_SeparatorHovered]     = EditorWidgets::steelBlue;
    c[ImGuiCol_SeparatorActive]      = EditorWidgets::steelBlueActive;
 
    // Resize grip — PURPLE (panel operation)
    c[ImGuiCol_ResizeGrip]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_ResizeGripHovered]    = EditorWidgets::steelBlueHover;
    c[ImGuiCol_ResizeGripActive]     = EditorWidgets::steelBlue;
 
    // Tabs — PURPLE identity
    c[ImGuiCol_Tab]                  = ImVec4(EditorWidgets::bg0.x + 0.01f, EditorWidgets::bg0.y + 0.01f, EditorWidgets::bg0.z + 0.01f, 0.92f);
    c[ImGuiCol_TabHovered]           = ImVec4(EditorWidgets::bg3.x, EditorWidgets::bg3.y + 0.01f, EditorWidgets::bg3.z + 0.03f, 1.00f);
    c[ImGuiCol_TabActive]            = EditorWidgets::purpleTabBg;
    c[ImGuiCol_TabUnfocused]         = ImVec4(EditorWidgets::bg0.x, EditorWidgets::bg0.y, EditorWidgets::bg0.z, 0.92f);
    c[ImGuiCol_TabUnfocusedActive]   = ImVec4(EditorWidgets::purpleTabBg.x - 0.03f, EditorWidgets::purpleTabBg.y - 0.02f, EditorWidgets::purpleTabBg.z - 0.03f, 0.88f);
 
    // Docking — PURPLE preview
    c[ImGuiCol_DockingPreview]       = ImVec4(EditorWidgets::purple.x, EditorWidgets::purple.y, EditorWidgets::purple.z, 0.45f);
    c[ImGuiCol_DockingEmptyBg]       = EditorWidgets::bg0;
 
    // Plot / data vis — TEAL lines, GREEN bars
    c[ImGuiCol_PlotLines]            = EditorWidgets::teal;
    c[ImGuiCol_PlotLinesHovered]     = ImVec4(EditorWidgets::teal.x + 0.05f, EditorWidgets::teal.y, EditorWidgets::teal.z - 0.05f, 1.00f);
    c[ImGuiCol_PlotHistogram]        = EditorWidgets::green;
    c[ImGuiCol_PlotHistogramHovered] = ImVec4(EditorWidgets::green.x + 0.05f, EditorWidgets::green.y - 0.05f, EditorWidgets::green.z + 0.05f, 1.00f);
 
    // Tables
    c[ImGuiCol_TableHeaderBg]        = EditorWidgets::bg0;
    c[ImGuiCol_TableBorderStrong]    = EditorWidgets::border;
    c[ImGuiCol_TableBorderLight]     = EditorWidgets::borderDim;
    c[ImGuiCol_TableRowBg]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.038f);
 
    // Drag & drop — STEEL BLUE (something in motion)
    c[ImGuiCol_DragDropTarget]       = ImVec4(EditorWidgets::steelBlue.x, EditorWidgets::steelBlue.y, EditorWidgets::steelBlue.z, 0.92f);
 
    // Nav — STEEL BLUE focus ring; PURPLE window switcher
    c[ImGuiCol_NavHighlight]          = EditorWidgets::steelBlue;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(EditorWidgets::purple.x, EditorWidgets::purple.y, EditorWidgets::purple.z, 0.80f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
 
    // Modal overlay
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.60f);
}

}  // namespace Atlas