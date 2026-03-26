#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace Atlas {

struct TileDefinition {
	std::string texturePath;
	glm::ivec2	gridIndex;	  // position in the spritesheet
	glm::ivec2	sizeInTiles;  // usually {1,1} for most tiles
	bool		isSolid;
};

class Tileset {
  public:
	Tileset(const std::string& name, const std::string& filepath) : m_name(name), m_filepath(filepath) {}

	std::unordered_map<int, TileDefinition>& getTileset() { return m_tileset; }

	void setName(const std::string& name) { m_name = name; }
	void setPath(const std::string& path) { m_filepath = path; }

	std::string getName() const { return m_name; }
	std::string getPath() const { return m_filepath; }

  private:
	std::unordered_map<int, TileDefinition> m_tileset;

	std::string m_name;
	std::string m_filepath;
};

}  // namespace Atlas