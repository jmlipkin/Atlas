#include "atpch.h"
#include "SubTexture.h"

#include "Atlas/Core/AssetManager.h"

namespace Atlas {

SubTexture::SubTexture(std::string texturePath, SubTextureSpecification specs) : m_texturePath(texturePath), m_specs(specs) {}

SubTexture::SubTexture(std::string texturePath, int tileSize, glm::ivec2 index, glm::vec2 size_in_tiles) : m_texturePath(texturePath) {
	std::shared_ptr<Texture> texture   = AssetManager::loadTexture(texturePath);
	const glm::vec2			 sheetSize = {texture->getWidth(), texture->getHeight()};

	m_specs.index		= index;
	m_specs.sizeInTiles = size_in_tiles;

	float textureWidth	= size_in_tiles.x * tileSize;
	float textureHeight = size_in_tiles.y * tileSize;

	TextureCoordinates subTexCoords;
	subTexCoords.top_left = glm::vec2(
		index.x * tileSize,
		index.y * tileSize);
	subTexCoords.top_right = glm::vec2(
		index.x * tileSize + textureWidth,
		index.y * tileSize);
	subTexCoords.bottom_left = glm::vec2(
		index.x * tileSize,
		index.y * tileSize + textureHeight);
	subTexCoords.bottom_right = glm::vec2(
		index.x * tileSize + textureWidth,
		index.y * tileSize + textureHeight);

	subTexCoords.top_left.x *= (1.0f / sheetSize.x);
	subTexCoords.top_left.y *= (1.0f / sheetSize.y);
	subTexCoords.top_right.x *= (1.0f / sheetSize.x);
	subTexCoords.top_right.y *= (1.0f / sheetSize.y);
	subTexCoords.bottom_left.x *= (1.0f / sheetSize.x);
	subTexCoords.bottom_left.y *= (1.0f / sheetSize.y);
	subTexCoords.bottom_right.x *= (1.0f / sheetSize.x);
	subTexCoords.bottom_right.y *= (1.0f / sheetSize.y);

	m_specs.coordinates = subTexCoords;
}

}  // namespace Atlas