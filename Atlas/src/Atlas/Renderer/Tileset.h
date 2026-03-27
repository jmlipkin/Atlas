#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace Atlas {

struct TileDefinition {
	glm::ivec2 gridIndex;	 // position in the spritesheet
	glm::ivec2 sizeInTiles;	 // usually {1,1} for most tiles
	bool	   isSolid;
};

class Tileset {
  public:
	Tileset(const std::string& name, const std::string& filepath) : m_name(name), m_filepath(filepath) {}

	std::unordered_map<int, TileDefinition>& getTileset() { return m_tileset; }

	void setName(const std::string& name) { m_name = name; }
	void setPath(const std::string& path) { m_filepath = path; }
	void setTexture(const std::string& texturePath) { m_texturePath = texturePath; }

	void setTile(int index, TileDefinition def) { m_tileset[index] = def; }
	void removeTile(int index) {
		if (m_tileset.contains(index)) m_tileset.erase(index);
	}

	std::string getName() const { return m_name; }
	std::string getPath() const { return m_filepath; }
	std::string getTexture() const { return m_texturePath; }

  private:
	std::unordered_map<int, TileDefinition> m_tileset;

	std::string m_name;
	std::string m_filepath;
	std::string m_texturePath;
};

}  // namespace Atlas