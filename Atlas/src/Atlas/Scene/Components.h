#pragma once

#include <glm/glm.hpp>

namespace Atlas {
namespace Component {

struct Tag {
    std::string tag;

    Tag() : tag("Unnamed entity") {}
    Tag(const std::string& tag_name) : tag(tag_name) {}
};

struct Transform {
    glm::vec3 position;
    float rotation;
    glm::vec2 size;
};

}  // namespace Component
}  // namespace Atlas