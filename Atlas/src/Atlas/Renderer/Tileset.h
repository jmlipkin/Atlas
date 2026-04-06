#pragma once

#include "Atlas/Renderer/SubTexture.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <functional>

namespace Atlas {

struct TileDefinition {
	glm::ivec2 gridIndex;	 // position in the spritesheet
	glm::ivec2 sizeInTiles;	 // usually {1,1} for most tiles
	bool	   isSolid;

	TextureCoordinates TexCoords() const {
		TextureCoordinates coords;
		coords.top_left		= gridIndex;
		coords.top_right	= {gridIndex.x + sizeInTiles.x, gridIndex.y};
		coords.bottom_left	= {gridIndex.x, gridIndex.y + sizeInTiles.y};
		coords.bottom_right = {gridIndex.x + sizeInTiles.x, gridIndex.y + sizeInTiles.y};
		return coords;
	}
};

class Tileset {
  public:
	using TileDeletedCallback = std::function<void()>;

	Tileset(const std::string& name, const std::string& filepath) : m_name(name), m_filepath(filepath) {}

	void setTileDeletedCallback(TileDeletedCallback callback) { m_deletedCallback = callback; }

	std::unordered_map<int, TileDefinition>& getTileset() { return m_tileset; }

	void setName(const std::string& name) { m_name = name; }
	void setPath(const std::string& path) { m_filepath = path; }
	void setTexture(const std::string& texturePath) { m_texturePath = texturePath; }

	void setTile(int index, TileDefinition def) { m_tileset[index] = def; }
	void removeTile(int index) {
		if (m_tileset.contains(index)) m_tileset.erase(index);
		m_deletedTile = index;
		if (m_deletedCallback) m_deletedCallback();
	}

	std::string getName() const { return m_name; }
	std::string getPath() const { return m_filepath; }
	std::string getTexture() const { return m_texturePath; }

	int	 getDeletedTile() const { return m_deletedTile; }
	void clearTileDeleted() { m_deletedTile = -1; }

  private:
	std::unordered_map<int, TileDefinition> m_tileset;

	std::string m_name;
	std::string m_filepath;
	std::string m_texturePath;

	TileDeletedCallback m_deletedCallback;
	int					m_deletedTile = -1;
};

}  // namespace Atlas