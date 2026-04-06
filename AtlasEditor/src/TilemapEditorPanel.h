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

	int getSelectedTile() const { return m_selectedTileIndex; }

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
		m_scene		   = scene;
		m_entity	   = entity;
		m_tilemap	   = tilemap;
		m_requestFocus = true;
		m_isOpen	   = true;

		m_zoom				= 1.0f;
		m_panOffset			= {0.0f, 0.0f};
		m_selectedTileIndex = -1;
		m_isPanning			= false;
		m_activeTool		= Tool::Paint;

		std::string path = ProjectManager::tilesetPath(tilemap->tileset);
		m_tileset		 = ProjectManager::loadTileset(path);

		// TODO: This should eventually be handled by ProjectManager
		m_tileset->setTileDeletedCallback([this]() {
			int deletedTile = m_tileset->getDeletedTile();
			m_tilemap->removeTile(deletedTile);
			m_tileset->clearTileDeleted();
		});

		if (m_tileset && !m_tileset->getTexture().empty())
			m_texture = AssetManager::loadTexture(m_tileset->getTexture());
		else
			m_texture = nullptr;

		m_tileSize = ProjectManager::getActiveProject()->getData().tileSize;

		m_palettePanel.open(m_tileset, m_texture);
	}

	void onImGuiRender() {
		if (!m_isOpen) return;

		m_selectedTileIndex = m_palettePanel.getSelectedTile();

		if (m_requestFocus) {
			ImGui::SetNextWindowFocus();
			m_requestFocus = false;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Tilemap Editor", &m_isOpen);

		drawTilemapHeader();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
		ImGui::BeginChild("Canvas", ImGui::GetContentRegionAvail());
		ImGui::PopStyleColor();

		drawCanvas();

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
		ImGui::SameLine();
		ImGui::SetCursorPosY(buttonPadding);

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::steelBlue);
		bool isPaint = m_activeTool == Tool::Paint;
		if (isPaint) ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::steelBlueActive);
		if (ImGui::Button("Paint")) m_activeTool = Tool::Paint;
		if (isPaint) ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetCursorPosY(buttonPadding);

		bool isErase = m_activeTool == Tool::Erase;
		if (isErase) ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::steelBlueActive);
		if (ImGui::Button("Erase")) m_activeTool = Tool::Erase;
		if (isErase) ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::EndDisabled();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	void drawCanvas() {
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();
		ImVec2 canvasPos  = ImGui::GetCursorScreenPos();

		ImGui::InvisibleButton("##canvas", canvasSize, ImGuiButtonFlags_MouseButtonRight);
		bool isHovered = ImGui::IsItemHovered();

		ImDrawList* dl = ImGui::GetWindowDrawList();

		float cellSize = m_tileSize * m_zoom * EditorWidgets::displayScale;
		int	  cols	   = m_tilemap->size.x;
		int	  rows	   = m_tilemap->size.y;

		float contentWidth	= cols * cellSize;
		float contentHeight = rows * cellSize;

		ImVec2 centerOffset = {0.0f, 0.0f};

		if (!m_hasUserPanned) {
			if (contentWidth < canvasSize.x)
				centerOffset.x = (canvasSize.x - contentWidth) * 0.5f;
			if (contentHeight < canvasSize.y)
				centerOffset.y = (canvasSize.y - contentHeight) * 0.5f;
		}

		ImVec2 totalOffset = {
			centerOffset.x + m_panOffset.x,
			centerOffset.y + m_panOffset.y};

		if (isHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			m_panOffset.x += delta.x;
			m_panOffset.y += delta.y;
			m_hasUserPanned = true;	 // <-- IMPORTANT
		}

		if (isHovered && ImGui::GetIO().MouseWheel != 0.0f) {
			ImVec2 mousePos = ImGui::GetIO().MousePos;

			// Convert mouse to "world" before zoom
			ImVec2 worldBefore = {
				mousePos.x - canvasPos.x - totalOffset.x,
				mousePos.y - canvasPos.y - totalOffset.y};

			float zoomBefore = m_zoom;
			m_zoom			 = glm::clamp(m_zoom + ImGui::GetIO().MouseWheel * 0.1f, 0.25f, 8.0f);
			float zoomRatio	 = m_zoom / zoomBefore;

			// Recompute sizes after zoom
			cellSize	  = m_tileSize * m_zoom * EditorWidgets::displayScale;
			contentWidth  = cols * cellSize;
			contentHeight = rows * cellSize;

			// Recompute centerOffset AFTER zoom
			if (!m_hasUserPanned) {
				centerOffset = {0, 0};
				if (contentWidth < canvasSize.x)
					centerOffset.x = (canvasSize.x - contentWidth) * 0.5f;
				if (contentHeight < canvasSize.y)
					centerOffset.y = (canvasSize.y - contentHeight) * 0.5f;
			}

			// Adjust pan so cursor stays fixed
			m_panOffset.x -= worldBefore.x * (zoomRatio - 1.0f);
			m_panOffset.y -= worldBefore.y * (zoomRatio - 1.0f);
		}

		// Final offset AFTER all updates
		totalOffset = {
			centerOffset.x + m_panOffset.x,
			centerOffset.y + m_panOffset.y};

		int hoveredCol = -1;
		int hoveredRow = -1;

		if (isHovered) {
			ImVec2 mousePos = ImGui::GetIO().MousePos;

			float worldX = mousePos.x - canvasPos.x - totalOffset.x;
			float worldY = mousePos.y - canvasPos.y - totalOffset.y;

			hoveredCol = (int)floor(worldX / cellSize);
			hoveredRow = (int)floor(worldY / cellSize);

			if (hoveredCol < 0 || hoveredCol >= cols || hoveredRow < 0 || hoveredRow >= rows) {
				hoveredCol = -1;
				hoveredRow = -1;
			}

			if (hoveredCol != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				m_tilemap->setTile(hoveredCol, hoveredRow, m_selectedTileIndex);
				ProjectManager::saveScene(m_scene);
			}
		}

		// In drawCanvas(), replace your painting logic with this:
		if (hoveredCol != -1 && hoveredRow != -1) {
			// Determine if we are painting or erasing
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (m_activeTool == Tool::Paint) {
					m_isPainting = true;
					m_tilemap->setTile(hoveredCol, hoveredRow, m_selectedTileIndex);
					m_sceneDirty = true;
				} else if (m_activeTool == Tool::Erase) {
					m_isErasing = true;
					m_tilemap->setTile(hoveredCol, hoveredRow, -1);
					m_sceneDirty = true;
				}
			} else {
				// Mouse released, commit any dirty changes
				if ((m_isPainting || m_isErasing) && m_sceneDirty) {
					ProjectManager::saveScene(m_scene);
					m_sceneDirty = false;
					m_isPainting = false;
					m_isErasing	 = false;
				}
			}
		}

		dl->PushClipRect(canvasPos, {canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y}, true);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				bool  isLight = ((col + row) % 2 == 0);
				ImU32 color	  = isLight
									? IM_COL32(45, 45, 45, 255)
									: IM_COL32(35, 35, 35, 255);

				ImVec2 cellMin = {
					canvasPos.x + totalOffset.x + col * cellSize,
					canvasPos.y + totalOffset.y + row * cellSize};
				ImVec2 cellMax = {
					cellMin.x + cellSize,
					cellMin.y + cellSize};

				dl->AddRectFilled(cellMin, cellMax, color);

				if (m_tilemap->getTile(col, row) != -1) {
					TileDefinition tile = m_tileset->getTileset().at(m_tilemap->getTile(col, row));

					float texW = m_texture->getWidth();
					float texH = m_texture->getHeight();

					glm::vec2 tl = tile.TexCoords().top_left;
					glm::vec2 br = tile.TexCoords().bottom_right;

					ImVec2 uv0 = {tl.x * m_tileSize / texW, tl.y * m_tileSize / texH};
					ImVec2 uv1 = {br.x * m_tileSize / texW, br.y * m_tileSize / texH};

					dl->AddImage(m_texture->getData(), cellMin, cellMax, uv0, uv1);
				}

				if (col == hoveredCol && row == hoveredRow) {
					dl->AddRect(cellMin, cellMax, ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlue));
				}
			}
		}

		dl->PopClipRect();
	}

  private:
	enum class Tool { Paint,
					  Erase };

	std::shared_ptr<Scene>	 m_scene;
	Entity*					 m_entity;
	Component::Tilemap*		 m_tilemap;
	std::shared_ptr<Tileset> m_tileset;
	int						 m_tileSize;
	bool					 m_isOpen;
	bool					 m_requestFocus;
	std::shared_ptr<Texture> m_texture;

	TilesetEditorPanel	m_tilesetPanel;
	TilemapPalettePanel m_palettePanel;

	float  m_zoom			   = 1.0f;
	ImVec2 m_panOffset		   = {0, 0};
	int	   m_selectedTileIndex = -1;
	bool   m_isPanning		   = false;

	Tool m_activeTool;

	bool m_hasUserPanned = false;
	bool m_sceneDirty	 = false;
	bool m_isPainting	 = false;
	bool m_isErasing	 = false;
};

}  // namespace Atlas