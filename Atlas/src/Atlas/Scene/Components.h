#pragma once

#include <glm/glm.hpp>

namespace Atlas {
namespace Component {

struct Tag {
    std::string name;
};

struct Transform {
    glm::vec3 position;
    float rotation;
    glm::vec size;
}

}  // namespace Component
}  // namespace Atlas