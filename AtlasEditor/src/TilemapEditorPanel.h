#pragma once

#include "TilesetEditorPanel.h"
#include "Commands/CommandHistory.h"

#include "Atlas/Renderer/Tileset.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"

#include "imgui/imgui.h"

namespace Atlas {

// ─────────────────────────────────────────────────────────────────────────────
//  TilemapPalettePanel
// ─────────────────────────────────────────────────────────────────────────────

class TilemapPalettePanel {
  public:
	void open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Texture> texture);

	void onImGuiRender();

	int	 getSelectedTile() const { return m_selectedTileIndex; }
	bool isOpen() const { return m_isOpen; }

  private:
	std::shared_ptr<Tileset> m_tileset;
	std::shared_ptr<Texture> m_texture;

	bool  m_isOpen			  = false;
	int	  m_selectedTileIndex = -1;
	float m_thumbnailSize	  = 48.0f;
};

// ─────────────────────────────────────────────────────────────────────────────
//  TilemapEditorPanel
// ─────────────────────────────────────────────────────────────────────────────

class TilemapEditorPanel {
  public:
	TilemapEditorPanel(CommandHistory& commandHistory) : m_commandHistory(commandHistory) {}

	void open(Component::Tilemap* tilemap, std::shared_ptr<Scene> scene, Entity* entity);

	void onImGuiRender();

  private:
	enum class Tool {
		Paint,
		Erase
	};

	void drawHeader();
	void drawCanvas();

	void handlePan(bool isActive);
	void handleZoom(bool isHovered, ImVec2 canvasPos, ImVec2 canvasSize, int cols, int rows);
	void handlePaint(int col, int row);

	void drawToolButton(const char* label, Tool tool, float padY);
	void drawCheckerboard(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset, float cellSize, int cols, int rows) const;
	void drawTiles(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset, float cellSize, int cols, int rows) const;
	void drawHoverHighlight(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset, float cellSize, int col, int row) const;

	void commitStroke();
	void save();

	std::pair<int, int> hoveredCell(bool isHovered, ImVec2 canvasPos, ImVec2 offset, float cellSize, int cols, int rows) const;

	ImVec2 totalOffset(ImVec2 canvasSize, int cols, int rows, float cellSize) const;

	static ImVec2 cellToScreen(ImVec2 canvasPos, ImVec2 offset, float cellSize, int col, int row);

  private:
	std::shared_ptr<Scene> m_scene;

	CommandHistory& m_commandHistory;

	Entity*				m_entity  = nullptr;
	Component::Tilemap* m_tilemap = nullptr;

	std::shared_ptr<Tileset> m_tileset;
	std::shared_ptr<Texture> m_texture;

	int m_tileSize = 0;

	TilesetEditorPanel	m_tilesetPanel;
	TilemapPalettePanel m_palettePanel;

	bool m_isOpen		= false;
	bool m_requestFocus = false;

	bool m_isPainting = false;
	bool m_isErasing  = false;
	Tool m_activeTool = Tool::Paint;

	std::vector<int> m_strokeSnapshot;

	bool   m_hasUserPanned = false;
	float  m_zoom		   = 1.0f;
	ImVec2 m_panOffset	   = {0.0f, 0.0f};

	bool m_sceneDirty = false;
};

}  // namespace Atlas