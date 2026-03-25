#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Project/Project.h"
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