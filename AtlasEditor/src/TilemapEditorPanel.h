#pragma once

#include "TilesetEditorPanel.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/Tileset.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

#include "Atlas/ImGui/EditorWidgets.h"
#include "imgui/imgui.h"

namespace Atlas {

class TilemapPalettePanel {
  public:
	void open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Texture> texture) {
		m_tileset = tileset;
		m_texture = texture;

		m_isOpen = true;
	}

	void onImGuiRender() {
		if (!m_isOpen) return;

		ImGui::Begin("Tilemap Palette", &m_isOpen);

		EditorWidgets::drawFloatSlider("", m_thumbnailSize, 0.0f, 120.0f, 48.0f, 12.0f, 96.0f, 1.0f);

		int	   tileSize = ProjectManager::getActiveProject()->getData().tileSize;
		ImVec2 size		= {m_thumbnailSize, m_thumbnailSize};

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float availWidth   = ImGui::GetContentRegionAvail().x;
		float cursorStartX = ImGui::GetCursorPosX();
		float currentX	   = cursorStartX;

		for (auto& [index, tile] : m_tileset->getTileset()) {
			float texW = (float)m_texture->getWidth();
			float texH = (float)m_texture->getHeight();

			ImGui::PushID(index);

			ImVec2 uv0 = {
				(tile.gridIndex.x * tileSize) / texW,
				(tile.gridIndex.y * tileSize) / texH};
			ImVec2 uv1 = {
				((tile.gridIndex.x + tile.sizeInTiles.x) * tileSize) / texW,
				((tile.gridIndex.y + tile.sizeInTiles.y) * tileSize) / texH};

			ImGui::InvisibleButton("##tile_N", size);

			if (ImGui::IsItemClicked(0)) {
				m_selectedTileIndex = index;
			}

			ImDrawList* dl = ImGui::GetWindowDrawList();

			ImVec2 tl = ImGui::GetItemRectMin();
			ImVec2 br = ImGui::GetItemRectMax();
			dl->AddImage(m_texture->getData(), tl, br, uv0, uv1);

			ImU32 col		= (m_selectedTileIndex == index) ? ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlueActive) : ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlueSub);
			float thickness = (m_selectedTileIndex == index) ? 2.5f : 1.0f;
			dl->AddRect(tl, br, col, 0.0f, 0, thickness);

			currentX += m_thumbnailSize;
			if (currentX + m_thumbnailSize <= cursorStartX + availWidth)
				ImGui::SameLine();
			else
				currentX = cursorStartX;

			ImGui::PopID();
		}
		ImGui::PopStyleVar();

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0)) {
			m_selectedTileIndex = -1;
		}

		ImGui::End();
	}

  private:
	std::shared_ptr<Tileset> m_tileset;
	std::shared_ptr<Texture> m_texture;

	bool m_isOpen			 = false;
	int	 m_selectedTileIndex = -1;

	float m_thumbnailSize = 48.0f;
};

class TilemapEditorPanel {
  public:
	void open(Component::Tilemap* tilemap, std::shared_ptr<Scene> scene, Entity* entity) {
		m_scene	  = scene;
		m_entity  = entity;
		m_tilemap = tilemap;
		m_isOpen  = true;

		m_zoom				= 1.0f;
		m_panOffset			= {0.0f, 0.0f};
		m_selectedTileIndex = -1;
		m_isPanning			= false;

		std::string path = ProjectManager::tilesetPath(tilemap->tileset);
		m_tileset		 = ProjectManager::loadTileset(path);

		if (m_tileset && !m_tileset->getTexture().empty())
			m_texture = AssetManager::loadTexture(m_tileset->getTexture());
		else
			m_texture = nullptr;

		m_tileSize = ProjectManager::getActiveProject()->getData().tileSize;

		m_palettePanel.open(m_tileset, m_texture);
	}

	void onImGuiRender() {
		if (!m_isOpen) return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Tilemap Editor", &m_isOpen);

		drawTilemapHeader();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
		ImGui::BeginChild("Canvas", ImGui::GetContentRegionAvail());
		ImGui::PopStyleColor();

		ImGui::Text("Child?");

		float cellSize = m_tileSize * m_zoom * EditorWidgets::displayScale;

		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleVar();

		m_tilesetPanel.onImGuiRender();
		m_palettePanel.onImGuiRender();
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
	int						 m_tileSize;
	bool					 m_isOpen;
	std::shared_ptr<Texture> m_texture;

	TilesetEditorPanel	m_tilesetPanel;
	TilemapPalettePanel m_palettePanel;

	float  m_zoom			   = 1.0f;
	ImVec2 m_panOffset		   = {0, 0};
	int	   m_selectedTileIndex = -1;
	bool   m_isPanning		   = false;
};

}  // namespace Atlas