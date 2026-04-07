#pragma once

#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/Tileset.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas {

class TilesetEditorPanel {
  public:
	void open(std::shared_ptr<Tileset> tileset, std::shared_ptr<Scene> scene);

	void onImGuiRender();
	void drawTilePanel();
	void drawTexturePicker();

	void autoSave();

  private:
	std::shared_ptr<Scene>	 m_scene;
	std::shared_ptr<Tileset> m_tileset;
	std::shared_ptr<Texture> m_texture;
	bool					 m_isOpen = false;

	int m_rows;
	int m_cols;
	int m_tileSize;

	int			   m_rightClickedIndex = -1;
	TileDefinition m_selectedTile;
	bool		   m_isTileSelected = false;
};

}  // namespace Atlas