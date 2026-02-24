#pragma once
#include "atpch.h"

#include "Atlas/Renderer/TextureSheet.h"
#include "Atlas/ECS/Entities/ScriptableEntity.h"

#include <glm/glm.hpp>

namespace Atlas::Component {

struct Tag {
	std::string tag;

	Tag() : tag("Unnamed entity") {}
	Tag(const std::string& tag_name) : tag(tag_name) {}
};

struct Transform {
	glm::vec3 position{0.0f};
	float rotation{0.0f};
	glm::vec2 size{1.0f};

	Transform() {}
	Transform(glm::vec3 pos, float rotate, glm::vec2 scale) : position(pos), rotation(rotate), size(scale) {}
};

struct Sprite {
	std::shared_ptr<SubTexture> subtexture;
	Sprite(std::shared_ptr<SubTexture> sprite) : subtexture(sprite) {}
};

struct Script {
	ScriptableEntity* instance = nullptr;

	ScriptableEntity* (*instantiateScript)();
	void (*destroyScript)(Script*);

	template <typename T>
	void bind() {
		instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
		destroyScript = [](Script* script) { delete script->instance; script->instance = nullptr; };
    }
};

}  // namespace Atlas::Component