#pragma once

#include <glm/glm.hpp>

namespace Components {

struct RigidBody {
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

}