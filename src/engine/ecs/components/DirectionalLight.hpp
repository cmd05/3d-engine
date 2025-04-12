#pragma once

#include <glm/glm.hpp>

namespace Components {

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 light_color;
};

}