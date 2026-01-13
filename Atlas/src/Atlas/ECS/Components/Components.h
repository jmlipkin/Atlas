#pragma once

#include <glm/glm.hpp>

#include "Atlas/Renderer/TextureSheet.h"

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

}  // namespace Atlas::Component