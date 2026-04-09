#include "atpch.h"
#include "Components.h"

#include "Atlas/Project/Project.h"
#include "Atlas/Renderer/SubTexture.h"

#include <glm/glm.hpp>

namespace Atlas::Component {

void Tilemap::resize(glm::ivec2 newSize) {
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

void Tilemap::removeTile(int deletedTile) {
	std::replace(grid.begin(), grid.end(), deletedTile, -1);
}

void Sprite::recalculateCoordinates() {
	if (texturePath.empty() || !ProjectManager::getActiveProject()) return;
	SubTexture sub(texturePath, ProjectManager::getActiveProject()->getData().tileSize, specs.index, specs.sizeInTiles);
	specs.coordinates = sub.getSpecs().coordinates;
}
void Sprite::recalculateCoordinates(int tileSize) {
	if (texturePath.empty()) return;
	SubTexture sub(texturePath, tileSize, specs.index, specs.sizeInTiles);
	specs.coordinates = sub.getSpecs().coordinates;
}

}  // namespace Atlas::Component
