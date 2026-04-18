#include "atpch.h"
#include "TilemapEditorPanel.h"

#include "Commands/LambdaCommand.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Project/Project.h"

#include "Atlas/ImGui/EditorWidgets.h"

namespace Atlas {

inline void tileUVs(const TileDefinition& tile, int tileSize, float texW, float texH,
					ImVec2& uv0, ImVec2& uv1) {
	uv0 = {(tile.gridIndex.x * tileSize) / texW,
		   (tile.gridIndex.y * tileSize) / texH};
	uv1 = {((tile.gridIndex.x + tile.sizeInTiles.x) * tileSize) / texW,
		   ((tile.gridIndex.y + tile.sizeInTiles.y) * tileSize) / texH};
}

// ─────────────────────────────────────────────────────────────────────────────
//  TilemapPalettePanel
// ─────────────────────────────────────────────────────────────────────────────

void TilemapPalettePanel::open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Texture> texture) {
	m_tileset			= tileset;
	m_texture			= texture;
	m_selectedTileIndex = -1;
	m_isOpen			= true;
}

void TilemapPalettePanel::onImGuiRender() {
	if (!m_isOpen) return;

	ImGui::Begin("Tilemap Palette", &m_isOpen);

	EditorWidgets::drawFloatSlider("", m_thumbnailSize, 0.0f, 120.0f, 48.0f, 12.0f, 96.0f, 1.0f);

	const int	 tileSize  = ProjectManager::getActiveProject()->getData().tileSize;
	const float	 texW	   = (float)m_texture->getWidth();
	const float	 texH	   = (float)m_texture->getHeight();
	const ImVec2 thumbSize = {m_thumbnailSize, m_thumbnailSize};

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	const float availWidth	 = ImGui::GetContentRegionAvail().x;
	const float cursorStartX = ImGui::GetCursorPosX();
	float		currentX	 = cursorStartX;

	for (auto& [index, tile] : m_tileset->getTileset()) {
		ImGui::PushID(index);

		ImVec2 uv0, uv1;
		tileUVs(tile, tileSize, texW, texH, uv0, uv1);

		ImGui::InvisibleButton("##tile", thumbSize);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_selectedTileIndex = index;

		ImDrawList* dl = ImGui::GetWindowDrawList();
		ImVec2		tl = ImGui::GetItemRectMin();
		ImVec2		br = ImGui::GetItemRectMax();

		dl->AddImage(m_texture->getData(), tl, br, uv0, uv1);

		bool  selected	= (m_selectedTileIndex == index);
		ImU32 borderCol = ImGui::ColorConvertFloat4ToU32(selected ? EditorWidgets::steelBlueActive
																  : EditorWidgets::steelBlueSub);
		dl->AddRect(tl, br, borderCol, 0.0f, 0, selected ? 2.5f : 1.0f);

		currentX += m_thumbnailSize;
		if (currentX + m_thumbnailSize <= cursorStartX + availWidth)
			ImGui::SameLine();
		else
			currentX = cursorStartX;

		ImGui::PopID();
	}

	ImGui::PopStyleVar();

	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		m_selectedTileIndex = -1;

	ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  TilemapEditorPanel
// ─────────────────────────────────────────────────────────────────────────────

void TilemapEditorPanel::open(Component::Tilemap* tilemap, std::shared_ptr<Scene> scene, Entity* entity) {
	m_tilemap		= tilemap;
	m_scene			= scene;
	m_entity		= entity;
	m_zoom			= 1.0f;
	m_panOffset		= {0.0f, 0.0f};
	m_hasUserPanned = false;
	m_isPainting	= false;
	m_isErasing		= false;
	m_sceneDirty	= false;
	m_activeTool	= Tool::Paint;
	m_requestFocus	= true;
	m_isOpen		= true;

	const std::string path = ProjectManager::tilesetPath(tilemap->tileset);
	m_tileset			   = ProjectManager::loadTileset(path);

	m_texture = (m_tileset && !m_tileset->getTexture().empty())
					? AssetManager::loadTexture(m_tileset->getTexture())
					: nullptr;

	m_tileSize = ProjectManager::getActiveProject()->getData().tileSize;

	m_palettePanel.open(m_tileset, m_texture);
}

void TilemapEditorPanel::onImGuiRender() {
	if (!m_isOpen) return;

	if (m_requestFocus) {
		ImGui::SetNextWindowFocus();
		m_requestFocus = false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Tilemap Editor", &m_isOpen);

	drawHeader();

	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
	ImGui::BeginChild("Canvas", ImGui::GetContentRegionAvail());
	ImGui::PopStyleColor();

	drawCanvas();

	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleVar();

	m_tilesetPanel.onImGuiRender();
	m_palettePanel.onImGuiRender();

	if (m_tileset && m_tileset->getDeletedTile() != -1) {
		m_tilemap->removeTile(m_tileset->getDeletedTile());
		m_tileset->clearTileDeleted();
		save();
	}
}

void TilemapEditorPanel::drawHeader() {
	const float headerH = 18.0f;
	const float frameH	= ImGui::GetFrameHeight();
	const float btnPadY = (headerH - frameH) * 0.5f;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorWidgets::bg0);
	ImGui::BeginChild("##tilemapheader", ImVec2(ImGui::GetContentRegionAvail().x, headerH));

	// Label
	float textPadY = (headerH - ImGui::GetTextLineHeight()) * 0.5f;
	ImGui::SetCursorPos({ImGui::GetCursorPosX() + 6.0f, textPadY});
	ImGui::Text("Editing Tilemap on Entity \"%s\"", m_entity->name().c_str());

	ImGui::BeginDisabled(!ProjectManager::getActiveProject());

	// Edit Tileset button
	ImGui::SameLine();
	ImGui::SetCursorPosY(btnPadY);
	if (ImGui::Button("Edit Tileset")) {
		m_tilesetPanel.open(AssetManager::get<Tileset>(m_tilemap->tileset), m_scene);
	}

	// Paint / Erase tool buttons
	drawToolButton("Paint", Tool::Paint, btnPadY);
	drawToolButton("Erase", Tool::Erase, btnPadY);

	ImGui::EndDisabled();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

void TilemapEditorPanel::drawCanvas() {
	const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	const ImVec2 canvasPos	= ImGui::GetCursorScreenPos();
	const int	 cols		= m_tilemap->size.x;
	const int	 rows		= m_tilemap->size.y;

	// Input capture — right mouse registered so dragging works
	ImGui::InvisibleButton("##canvas", canvasSize, ImGuiButtonFlags_MouseButtonRight);
	const bool isHovered = ImGui::IsItemHovered();
	const bool isActive	 = ImGui::IsItemActive();

	handlePan(isActive);
	handleZoom(isHovered, canvasPos, canvasSize, cols, rows);

	const float	 cellSize = m_tileSize * m_zoom * EditorWidgets::displayScale;
	const ImVec2 offset	  = totalOffset(canvasSize, cols, rows, cellSize);

	// Tile under cursor
	const auto [hovCol, hovRow] = hoveredCell(isHovered, canvasPos, offset, cellSize, cols, rows);

	handlePaint(hovCol, hovRow);

	// Draw
	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->PushClipRect(canvasPos, {canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y}, true);

	drawCheckerboard(dl, canvasPos, offset, cellSize, cols, rows);
	drawTiles(dl, canvasPos, offset, cellSize, cols, rows);
	drawHoverHighlight(dl, canvasPos, offset, cellSize, hovCol, hovRow);

	dl->PopClipRect();
}

void TilemapEditorPanel::handlePan(bool isActive) {
	if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		float  dist	 = glm::length(glm::vec2(ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).x,
											 ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).y));
		if (dist > 3.0f * EditorWidgets::displayScale) {
			m_panOffset.x += delta.x;
			m_panOffset.y += delta.y;
			m_hasUserPanned = true;
		}
	}
}

void TilemapEditorPanel::handleZoom(bool isHovered, ImVec2 canvasPos, ImVec2 canvasSize, int cols, int rows) {
	if (!isHovered || ImGui::GetIO().MouseWheel == 0.0f) return;

	const float	 cellSize = m_tileSize * m_zoom * EditorWidgets::displayScale;
	const ImVec2 off	  = totalOffset(canvasSize, cols, rows, cellSize);
	ImVec2		 mouse	  = ImGui::GetIO().MousePos;

	// World position under cursor before zoom
	glm::vec2 worldBefore = {
		mouse.x - canvasPos.x - off.x,
		mouse.y - canvasPos.y - off.y};

	const float prevZoom = m_zoom;
	m_zoom				 = glm::clamp(m_zoom + ImGui::GetIO().MouseWheel * 0.1f, 0.25f, 8.0f);

	const float ratio = m_zoom / prevZoom;
	m_panOffset.x -= worldBefore.x * (ratio - 1.0f);
	m_panOffset.y -= worldBefore.y * (ratio - 1.0f);
}

void TilemapEditorPanel::handlePaint(int col, int row) {
	if (col == -1) {
		if ((m_isPainting || m_isErasing) && m_sceneDirty)
			commitStroke();
		return;
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		// Capture snapshot on first frame of stroke
		if (!m_isPainting && !m_isErasing)
			m_strokeSnapshot = m_tilemap->grid;

		if (m_activeTool == Tool::Paint) {
			m_tilemap->setTile(col, row, m_palettePanel.getSelectedTile());
			m_isPainting = true;
			m_sceneDirty = true;
		} else if (m_activeTool == Tool::Erase) {
			m_tilemap->setTile(col, row, -1);
			m_isErasing	 = true;
			m_sceneDirty = true;
		}
	} else if ((m_isPainting || m_isErasing) && m_sceneDirty) {
		commitStroke();
	}
}

void TilemapEditorPanel::drawToolButton(const char* label, Tool tool, float padY) {
	ImGui::SameLine();
	ImGui::SetCursorPosY(padY);

	bool active = (m_activeTool == tool);
	if (active) ImGui::PushStyleColor(ImGuiCol_Button, EditorWidgets::steelBlueActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorWidgets::steelBlue);

	if (ImGui::Button(label))
		m_activeTool = tool;

	ImGui::PopStyleColor(active ? 2 : 1);
}

void TilemapEditorPanel::drawCheckerboard(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset,
										  float cellSize, int cols, int rows) const {
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			ImU32  color   = ((col + row) % 2 == 0) ? IM_COL32(45, 45, 45, 255)
													: IM_COL32(35, 35, 35, 255);
			ImVec2 cellMin = cellToScreen(canvasPos, offset, cellSize, col, row);
			ImVec2 cellMax = {cellMin.x + cellSize, cellMin.y + cellSize};
			dl->AddRectFilled(cellMin, cellMax, color);
		}
	}
}

void TilemapEditorPanel::drawTiles(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset,
								   float cellSize, int cols, int rows) const {
	if (!m_texture) return;

	const float texW = (float)m_texture->getWidth();
	const float texH = (float)m_texture->getHeight();

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			const int tileIndex = m_tilemap->getTile(col, row);
			if (tileIndex == -1) continue;

			const TileDefinition& tile = m_tileset->getTileset().at(tileIndex);
			ImVec2				  uv0, uv1;
			tileUVs(tile, m_tileSize, texW, texH, uv0, uv1);

			ImVec2 cellMin = cellToScreen(canvasPos, offset, cellSize, col, row);
			ImVec2 cellMax = {cellMin.x + cellSize, cellMin.y + cellSize};
			dl->AddImage(m_texture->getData(), cellMin, cellMax, uv0, uv1);
		}
	}
}

void TilemapEditorPanel::drawHoverHighlight(ImDrawList* dl, ImVec2 canvasPos, ImVec2 offset,
											float cellSize, int col, int row) const {
	if (col == -1) return;
	ImVec2 cellMin = cellToScreen(canvasPos, offset, cellSize, col, row);
	ImVec2 cellMax = {cellMin.x + cellSize, cellMin.y + cellSize};
	dl->AddRect(cellMin, cellMax, ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlue));
}

void TilemapEditorPanel::commitStroke() {
	std::vector<int> beforeGrid = m_strokeSnapshot;
	std::vector<int> afterGrid	= m_tilemap->grid;
	std::string		 name		= m_isPainting ? "Paint Tiles" : "Erase Tiles";
	Entity			 entity		= *m_entity;

	m_commandHistory.push(std::make_unique<LambdaCommand>(name, [entity, afterGrid]() mutable {
            entity.refresh();
            entity.getComponent<Component::Tilemap>().grid = afterGrid; }, [entity, beforeGrid]() mutable {
            entity.refresh();
            entity.getComponent<Component::Tilemap>().grid = beforeGrid; }));

	save();
}

void TilemapEditorPanel::save() {
	ProjectManager::saveScene(m_scene);
	m_sceneDirty = false;
	m_isPainting = false;
	m_isErasing	 = false;
}

std::pair<int, int> TilemapEditorPanel::hoveredCell(bool isHovered, ImVec2 canvasPos, ImVec2 offset,
													float cellSize, int cols, int rows) const {
	if (!isHovered) return {-1, -1};

	ImVec2 mouse = ImGui::GetIO().MousePos;
	int	   col	 = (int)floor((mouse.x - canvasPos.x - offset.x) / cellSize);
	int	   row	 = (int)floor((mouse.y - canvasPos.y - offset.y) / cellSize);

	if (col < 0 || col >= cols || row < 0 || row >= rows)
		return {-1, -1};

	return {col, row};
}

ImVec2 TilemapEditorPanel::totalOffset(ImVec2 canvasSize, int cols, int rows, float cellSize) const {
	ImVec2 center = {0.0f, 0.0f};
	if (!m_hasUserPanned) {
		const float contentW = cols * cellSize;
		const float contentH = rows * cellSize;
		if (contentW < canvasSize.x) center.x = (canvasSize.x - contentW) * 0.5f;
		if (contentH < canvasSize.y) center.y = (canvasSize.y - contentH) * 0.5f;
	}
	return {center.x + m_panOffset.x, center.y + m_panOffset.y};
}

ImVec2 TilemapEditorPanel::cellToScreen(ImVec2 canvasPos, ImVec2 offset, float cellSize, int col, int row) {
	return {
		canvasPos.x + offset.x + col * cellSize,
		canvasPos.y + offset.y + row * cellSize};
}

}  // namespace Atlas