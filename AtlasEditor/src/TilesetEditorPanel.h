#pragma once

#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/Platform.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/Tileset.h"

#include "Atlas/ImGui/EditorWidgets.h"
#include "Atlas/Scene/Scene.h"
#include "imgui/imgui.h"

#include <cmath>
#include <memory>

namespace Atlas {

class TilesetEditorPanel {
  public:
	void open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Scene> scene) {
		m_scene	  = scene;
		m_tileset = tileset;
		m_isOpen  = true;

		m_tileSize = ProjectManager::getActiveProject()->getData().tileSize;

		if (!tileset->getTexture().empty()) {
			m_texture = AssetManager::loadTexture(tileset->getTexture());
		} else {
			m_texture = nullptr;
		}
	}

	void onImGuiRender() {
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

		ImDrawList* dl	 = ImGui::GetWindowDrawList();
		int			rows = ceil((float)m_texture->getHeight() / (float)m_tileSize);
		int			cols = ceil((float)m_texture->getWidth() / (float)m_tileSize);

		// Adjust the starting position based on the cursor position

		for (int i = 0; i <= rows; i++) {
			// Calculate the min and max points for the current cell
			const ImVec2 markerMin = ImVec2(startPos.x, startPos.y + i * m_tileSize * scale);
			const ImVec2 markerMax = ImVec2(startPos.x + finalSize.x, startPos.y + i * m_tileSize * scale);

			// Add a filled rectangle to the draw list
			dl->AddLine(markerMin, markerMax, ImGui::ColorConvertFloat4ToU32({EditorWidgets::steelBlue.x, EditorWidgets::steelBlue.y, EditorWidgets::steelBlue.z, 0.5f}), 1.5f);
		}

		for (int j = 0; j <= cols; j++) {
			// Calculate the min and max points for the current cell
			const ImVec2 markerMin = ImVec2(startPos.x + j * m_tileSize * scale, startPos.y);
			const ImVec2 markerMax = ImVec2(startPos.x + j * m_tileSize * scale, startPos.y + finalSize.y);

			// Add a filled rectangle to the draw list
			dl->AddLine(markerMin, markerMax, ImGui::ColorConvertFloat4ToU32({EditorWidgets::steelBlue.x, EditorWidgets::steelBlue.y, EditorWidgets::steelBlue.z, 0.5f}), 1.5f);
		}

		ImGui::End();
	}

	void drawTexturePicker() {
		std::string filepath = "No texture";
		if (m_texture) {
			filepath = ProjectManager::toRelativePath(m_texture->getFilepath());
		}

		float changeButtonWidth = 62.0f * EditorWidgets::displayScale;
		float maxWidth			= ImGui::GetContentRegionAvail().x - changeButtonWidth - 8.0f * EditorWidgets::displayScale;

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
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - changeButtonWidth - 4.0f * EditorWidgets::displayScale);
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

  private:
	std::shared_ptr<Scene>	 m_scene;
	std::shared_ptr<Tileset> m_tileset;
	std::shared_ptr<Texture> m_texture;
	bool					 m_isOpen = false;

	int			   m_tileSize;
	TileDefinition m_selectedTile;
};

}  // namespace Atlas