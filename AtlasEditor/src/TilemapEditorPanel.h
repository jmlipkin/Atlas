#pragma once

#include "TilesetEditorPanel.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/Tileset.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

#include "Atlas/ImGui/EditorWidgets.h"
#include "imgui/imgui.h"

#include <memory>

namespace Atlas {

class TilemapEditorPanel {
  public:
	void open(Component::Tilemap* tilemap, std::shared_ptr<Scene> scene, Entity* entity) {
		m_scene	  = scene;
		m_entity  = entity;
		m_tilemap = tilemap;
		m_isOpen  = true;

		std::string path = ProjectManager::tilesetPath(tilemap->tileset);
		m_tileset = ProjectManager::loadTileset(path);
	}

	void onImGuiRender() {
		if (!m_isOpen) return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Tilemap Editor", &m_isOpen);

		drawTilemapHeader();

		ImGui::End();
		ImGui::PopStyleVar();

		m_tilesetPanel.onImGuiRender();
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
			std::shared_ptr<Tileset> tileset = AssetManager::get<Tileset>(m_tilemap->tileset);
			m_tilesetPanel.open(tileset, m_scene);
		}

		ImGui::EndDisabled();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

  private:
	std::shared_ptr<Scene>	 m_scene;
	Entity*					 m_entity;
	Component::Tilemap*		 m_tilemap;
	std::shared_ptr<Tileset> m_tileset;
	bool					 m_isOpen;

	TilesetEditorPanel m_tilesetPanel;
};

}  // namespace Atlas