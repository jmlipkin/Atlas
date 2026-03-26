#pragma once

#include "Atlas/Core/Log.h"
#include "Atlas/Core/AssetManager.h"
#include "Atlas/Renderer/Tileset.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

#include "Atlas/ImGui/EditorWidgets.h"
#include "imgui/imgui.h"

namespace Atlas {

class TilemapEditorPanel {
  public:
	void open(Component::Tilemap* tilemap, Entity* entity) {
		m_entity  = entity;
		m_tilemap = tilemap;
		m_tileset = AssetManager::get<Tileset>(tilemap->tileset);
		m_isOpen  = true;
	}

	void onImGuiRender() {
		if (!m_isOpen) return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Tilemap Editor", &m_isOpen);

		drawTilemapHeader();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void drawTilemapHeader() {
		float headerHeight = 18.0f * EditorWidgets::displayScale;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorWidgets::bg0);
		ImGui::BeginChild("##tilemapheader", ImVec2(ImGui::GetContentRegionAvail().x, headerHeight));

		// Vertically center content
		float textHeight	  = ImGui::GetTextLineHeight();
		float verticalPadding = (headerHeight - textHeight) * 0.5f;
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 6.0f * EditorWidgets::displayScale, verticalPadding));
		ImGui::Text("Editing Tilemap on Entity \"%s\"", m_entity->name().c_str());

		float buttonHeight	= ImGui::GetFrameHeight();
		float buttonPadding = (headerHeight - buttonHeight) * 0.5f;
        ImGui::SameLine();
		ImGui::SetCursorPosY(buttonPadding);

		ImGui::BeginDisabled(!ProjectManager::getActiveProject());

		if (ImGui::Button("Edit Tileset")) {
			AT_CORE_WARN("Cannot edit tilesets yet");
		}

		ImGui::EndDisabled();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

  private:
	Entity*					 m_entity;
	Component::Tilemap*		 m_tilemap;
	std::shared_ptr<Tileset> m_tileset;
	bool					 m_isOpen;
};

}  // namespace Atlas