#pragma once

#include <glm/glm.hpp>

namespace ngn {

struct __attribute__((packed)) Vertex {
    glm::vec3 position { 0 };
    glm::vec3 normal { 0, 0, 1 };
    glm::vec2 texture_coordinates { 0 };
};

}
