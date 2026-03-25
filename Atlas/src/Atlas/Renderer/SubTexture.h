#pragma once

#include "Atlas/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Atlas {

struct TextureCoordinates {
	glm::vec2 top_left {0, 1};
	glm::vec2 top_right {1, 1};
	glm::vec2 bottom_left {0, 0};
	glm::vec2 bottom_right {1, 0};

	bool operator==(const TextureCoordinates& other) const {
		bool equal = true;
		equal &= top_left == other.top_left;
		equal &= top_right == other.top_right;
		equal &= bottom_left == other.bottom_left;
		equal &= bottom_right == other.bottom_right;
		return equal;
	}
};

struct SubTextureSpecification {
	TextureCoordinates coordinates;
	glm::vec2		   sizeInTiles;
	glm::ivec2		   index;
};

class SubTexture {
  public:
	SubTexture(std::string texturePath, SubTextureSpecification specs);
	SubTexture(std::string texturePath, int tileSize, glm::ivec2 index, glm::vec2 size_in_tiles = glm::vec2(1.0f));
	virtual ~SubTexture() = default;

	const std::string		  getTexturePath() const { return m_texturePath; }
	const TextureCoordinates& getTexCoords() const { return m_specs.coordinates; }

	float getNumTilesHorizontal() const { return m_specs.sizeInTiles.x; }
	float getNumTilesVertical() const { return m_specs.sizeInTiles.y; }

	glm::vec2				 getSizeInTiles() const { return m_specs.sizeInTiles; }
	SubTextureSpecification& getSpecs() { return m_specs; }

  private:
	std::string				m_texturePath;
	SubTextureSpecification m_specs;
};

}  // namespace Atlas