#include "atpch.h"
#include "TilesetEditorPanel.h"

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"

#include "Atlas/ImGui/EditorWidgets.h"

#include <imgui/imgui.h>

namespace Atlas {

void TilesetEditorPanel::open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Scene> scene) {
	m_scene			 = scene;
	m_tileset		 = tileset;
	m_isTileSelected = false;
	m_isOpen		 = true;

	if (!tileset->getTexture().empty()) {
		m_texture = AssetManager::loadTexture(tileset->getTexture());
	} else {
		m_texture = nullptr;
	}

	m_tileSize = ProjectManager::getActiveProject()->getData().tileSize;
	m_cols	   = ceil((float)m_texture->getWidth() / (float)m_tileSize);
	m_rows	   = ceil((float)m_texture->getHeight() / (float)m_tileSize);
}

void TilesetEditorPanel::onImGuiRender() {
	if (!m_isOpen) return;
	ImGui::Begin("Tileset Editor", &m_isOpen);

	drawTexturePicker();

	if (!m_texture) {
		ImGui::End();
		return;
	}

	float  aspectRatio		  = (float)m_texture->getWidth() / (float)m_texture->getHeight();
	ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

	float  contentRegionAspectRatio = contentRegionAvail.x / contentRegionAvail.y;
	ImVec2 finalSize;
	if (contentRegionAspectRatio > aspectRatio) {
		finalSize.y = contentRegionAvail.y;
		finalSize.x = finalSize.y * aspectRatio;
	} else {
		finalSize.x = contentRegionAvail.x;
		finalSize.y = finalSize.x / aspectRatio;
	}
	ImVec2 cursorPos = ImGui::GetCursorPos();
	float  scale	 = finalSize.x / (float)m_texture->getWidth();

	ImVec2 padding;
	padding.x = (contentRegionAvail.x - finalSize.x) * 0.5f;
	padding.y = (contentRegionAvail.y - finalSize.y) * 0.5f;
	ImGui::SetCursorPos(ImVec2(cursorPos.x + padding.x, cursorPos.y + padding.y));
	ImVec2 startPos = ImGui::GetCursorScreenPos();

	void* data = m_texture->getData();
	ImGui::Image(data, finalSize);

	ImDrawList* dl = ImGui::GetWindowDrawList();

	// Highlight added tiles
	for (auto [index, tile] : m_tileset->getTileset()) {
		const ImVec2 rectMin = ImVec2(startPos.x + tile.gridIndex.x * m_tileSize * scale, startPos.y + tile.gridIndex.y * m_tileSize * scale);
		const ImVec2 rectMax = ImVec2(startPos.x + (tile.gridIndex.x + 1) * m_tileSize * scale, startPos.y + (tile.gridIndex.y + 1) * m_tileSize * scale);
		dl->AddRectFilled(rectMin, rectMax, ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlueHover));
	}

	// Adjust the starting position based on the cursor position

	for (int i = 0; i <= m_rows; i++) {
		// Calculate the min and max points for the current cell
		const ImVec2 markerMin = ImVec2(startPos.x, startPos.y + i * m_tileSize * scale);
		const ImVec2 markerMax = ImVec2(startPos.x + finalSize.x, startPos.y + i * m_tileSize * scale);

		// Add a filled rectangle to the draw list
		dl->AddLine(markerMin, markerMax, ImGui::ColorConvertFloat4ToU32(EditorWidgets::steelBlueSub), 1.5f);
	}

	for (int j = 0; j <= m_cols; j++) {
		// Calculate the min and max points for the current cell
		const ImVec2 markerMin = ImVec2(startPos.x + j * m_tileSize * scale, startPos.y);
		const ImVec2 markerMax = ImVec2(startPos.x + j * m_tileSize * scale, startPos.y + finalSize.y);

		// Add a filled rectangle to the draw list
		dl->AddLine(markerMin, markerMax, ImGui::ColorConvertFloat4ToU32({EditorWidgets::steelBlue.x, EditorWidgets::steelBlue.y, EditorWidgets::steelBlue.z, 0.5f}), 1.0f);
	}

	// Highlight selected cell
	if (m_isTileSelected) {
		dl->AddRect({startPos.x + m_tileSize * scale * m_selectedTile.gridIndex.x, startPos.y + m_tileSize * scale * m_selectedTile.gridIndex.y}, {startPos.x + m_tileSize * scale * (m_selectedTile.gridIndex.x + 1), startPos.y + m_tileSize * scale * (m_selectedTile.gridIndex.y + 1)}, ImGui::ColorConvertFloat4ToU32(EditorWidgets::greenPanelAccent), 0, 0, 2.5f);
	}

	if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) {
		ImVec2 adjustedMousePos(ImGui::GetMousePos().x - startPos.x, ImGui::GetMousePos().y - startPos.y);

		int row	   = floor(adjustedMousePos.y / (m_tileSize * scale));
		int column = floor(adjustedMousePos.x / (m_tileSize * scale));

		if (row >= 0 && column >= 0 && row < m_rows && column < m_cols) {
			bool leftClicked = false;

			leftClicked = ImGui::IsMouseClicked(0);

			if (leftClicked) {
				m_isTileSelected = true;

				int index = column + row * m_cols;
				if (m_tileset->getTileset().contains(index))
					m_selectedTile = m_tileset->getTileset()[index];
				else {
					m_selectedTile			   = {};
					m_selectedTile.gridIndex   = {column, row};
					m_selectedTile.sizeInTiles = {1, 1};
				}
			}

			ImVec4 color = (leftClicked) ? EditorWidgets::steelBlueLight : ImVec4{EditorWidgets::steelBlue.x, EditorWidgets::steelBlue.y, EditorWidgets::steelBlue.z, 0.5f};

			dl->AddRect({startPos.x + m_tileSize * scale * column, startPos.y + m_tileSize * scale * row}, {startPos.x + m_tileSize * scale * (column + 1), startPos.y + m_tileSize * scale * (row + 1)}, ImGui::ColorConvertFloat4ToU32(color), 0, 0, 2.5f);

			if (ImGui::IsMouseClicked(1)) {
				m_rightClickedIndex = column + row * m_cols;
				ImGui::OpenPopup("##tilecontext");
			}
		}
	}

	if (ImGui::BeginPopup("##tilecontext")) {
		if (m_tileset->getTileset().contains(m_rightClickedIndex)) {
			if (ImGui::MenuItem("Remove")) {
				m_tileset->removeTile(m_rightClickedIndex);
				if (m_isTileSelected) {
					int selectedIndex = m_selectedTile.gridIndex.x + m_selectedTile.gridIndex.y * m_cols;
					if (selectedIndex == m_rightClickedIndex)
						m_isTileSelected = false;
				}
				autoSave();
			}
		} else {
			if (ImGui::MenuItem("Add Tile")) {
				TileDefinition def;
				def.gridIndex	= {m_rightClickedIndex % m_cols, m_rightClickedIndex / m_cols};
				def.sizeInTiles = {1, 1};
				def.isSolid		= false;
				m_tileset->setTile(m_rightClickedIndex, def);
				m_selectedTile	 = def;
				m_isTileSelected = true;
				autoSave();
			}
		}
		ImGui::EndPopup();
	}

	ImGui::End();

	if (m_isTileSelected) {
		drawTilePanel();
	}
}

void TilesetEditorPanel::drawTilePanel() {
	ImGui::Begin("Tile Editor");
	float columnWidth = 50.0f;
	float valueWidth  = 85.0f;

	EditorWidgets::drawVec2Control<glm::ivec2>("Index", m_selectedTile.gridIndex, columnWidth, valueWidth);

	EditorWidgets::drawVec2Control("Size", m_selectedTile.sizeInTiles, columnWidth, valueWidth, 1.0f, 1.0f);

	EditorWidgets::drawCheckbox("Solid", m_selectedTile.isSolid, columnWidth);

	if (ImGui::Button("Apply")) {
		int index = m_selectedTile.gridIndex.y * m_cols + m_selectedTile.gridIndex.x;
		m_tileset->setTile(index, m_selectedTile);
		autoSave();
	}
	ImGui::End();
}

void TilesetEditorPanel::drawTexturePicker() {
	std::string filepath = "No texture";
	if (m_texture) {
		filepath = ProjectManager::toRelativePath(m_texture->getFilepath());
	}

	float changeButtonWidth = 62.0f;
	float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - 8.0f;

	std::string displayPath = filepath.empty() ? "No texture" : filepath;
	if (!filepath.empty() && ImGui::CalcTextSize(filepath.c_str()).x > maxWidth) {
		std::string truncated = std::filesystem::path(filepath).filename().string();
		while (!truncated.empty() && ImGui::CalcTextSize((truncated + "...").c_str()).x > maxWidth)
			truncated.pop_back();
		displayPath = truncated + "...";
	}

	ImGui::Text("%s", displayPath.c_str());
	if (ImGui::IsItemHovered() && !filepath.empty())
		ImGui::SetTooltip("%s", filepath.c_str());

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f);
	if (ImGui::Button("Change", ImVec2(changeButtonWidth, 0))) {
		const std::string& texturePath = Platform::openFileDialog("png");
		if (!texturePath.empty()) {
			m_tileset->setTexture(texturePath);
			m_texture	   = AssetManager::loadTexture(texturePath);
			m_selectedTile = {};
			ProjectManager::saveTileset(m_tileset);
			ProjectManager::saveScene(m_scene);
		}
	}
}

void TilesetEditorPanel::autoSave() {
	ProjectManager::saveTileset(m_tileset);
}
}  // namespace Atlas