#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Renderer/SubTexture.h"

#include <glm/glm.hpp>

namespace Atlas::Component {

struct UUID {
	Atlas::UUID id;
	UUID() = default;
	UUID(Atlas::UUID uuid) : id(uuid) {}
};
struct Tag {
	std::string tag;

	Tag() : tag("Unnamed entity") {}
	Tag(const std::string& tag_name) : tag(tag_name) {}
};

struct Tilemap {
	glm::ivec2	size = {1, 1};
	std::string tileset;
	uint32_t	layerMask = 0xFFFFFFFF;

	bool showOverlay = false;

	std::vector<int> grid = {-1, -1};

	int	 getTile(uint32_t x, uint32_t y) const { return grid[y * size.x + x]; }
	void setTile(uint32_t x, uint32_t y, int index) { grid[y * size.x + x] = index; }

	void resize(glm::ivec2 newSize);
	void removeTile(int deletedTile);
};

struct Transform {
	glm::vec3 position{0.0f, 0.0f, 1.0f};
	float	  rotation{0.0f};
	glm::vec2 size{1.0f};

	Transform() {}
	Transform(glm::vec3 pos, float rotate, glm::vec2 scale) : position(pos), rotation(rotate), size(scale) {}
};

struct Sprite {
	std::string				texturePath;
	SubTextureSpecification specs;

	Sprite() = default;
	Sprite(std::string path, SubTextureSpecification texSpecs) : texturePath(path), specs(texSpecs) {}
	Sprite(std::shared_ptr<SubTexture> sprite) : texturePath(sprite->getTexturePath()), specs(sprite->getSpecs()) {}

	void recalculateCoordinates();
	void recalculateCoordinates(int tileSize);
};

}  // namespace Atlas::Component