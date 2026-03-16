#include "atpch.h"
#include "SubTexture.h"

#include "Atlas/Core/AssetManager.h"

namespace Atlas {

SubTexture::SubTexture(std::string texturePath, SubTextureSpecification specs) : m_texturePath(texturePath), m_specs(specs) {}

SubTexture::SubTexture(std::string texturePath, glm::vec2 tileSize, glm::ivec2 index, glm::vec2 size_in_tiles) : m_texturePath(texturePath) {
	std::shared_ptr<Texture> texture   = AssetManager::loadTexture(texturePath);
	const glm::vec2			 sheetSize = {texture->getWidth(), texture->getHeight()};

	m_specs.index		= index;
	m_specs.sizeInTiles = size_in_tiles;
	m_specs.tileSize	= tileSize;

	float textureWidth	= size_in_tiles.x * tileSize.x;
	float textureHeight = size_in_tiles.y * tileSize.y;

	TextureCoordinates subTexCoords;
	subTexCoords.top_left = glm::vec2(
		index.x * tileSize.x,
		index.y * tileSize.y);
	subTexCoords.top_right = glm::vec2(
		index.x * tileSize.x + textureWidth,
		index.y * tileSize.y);
	subTexCoords.bottom_left = glm::vec2(
		index.x * tileSize.x,
		index.y * tileSize.y + textureHeight);
	subTexCoords.bottom_right = glm::vec2(
		index.x * tileSize.x + textureWidth,
		index.y * tileSize.y + textureHeight);

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