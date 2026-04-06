#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/SubTexture.h"

#include <glm/glm.hpp>
#include <algorithm>

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
	int			layer;

	bool showOverlay;

	std::vector<int> grid = {-1, -1};

	int getTile(uint32_t x, uint32_t y) { return grid[y * size.x + x]; }
	int setTile(uint32_t x, uint32_t y, uint32_t index) { return grid[y * size.x + x] = index; }

	void resize(glm::ivec2 newSize) {
		std::vector<int> newGrid(newSize.x * newSize.y, -1);

		int copyWidth  = std::min(size.x, newSize.x);
		int copyHeight = std::min(size.y, newSize.y);

		for (int y = 0; y < copyHeight; y++) {
			for (int x = 0; x < copyWidth; x++) {
				newGrid[y * newSize.x + x] = grid[y * size.x + x];
			}
		}

		size = newSize;
		grid = newGrid;
	}

	void removeTile(int deletedTile) {
		std::replace(grid.begin(), grid.end(), deletedTile, -1);
	}
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

	void recalculateCoordinates() {
		if (texturePath.empty()) return;
		SubTexture sub(texturePath, ProjectManager::getActiveProject()->getData().tileSize, specs.index, specs.sizeInTiles);
		specs.coordinates = sub.getSpecs().coordinates;
	}
	void recalculateCoordinates(int tileSize) {
		if (texturePath.empty()) return;
		SubTexture sub(texturePath, tileSize, specs.index, specs.sizeInTiles);
		specs.coordinates = sub.getSpecs().coordinates;
	}
};

}  // namespace Atlas::Component