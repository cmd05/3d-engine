#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/components/Transform.hpp>

constexpr glm::vec3 MATH_X_AXIS {1.0f, 0.0f, 0.0f};
constexpr glm::vec3 MATH_Y_AXIS {0.0f, 1.0f, 0.0f};
constexpr glm::vec3 MATH_Z_AXIS {0.0f, 0.0f, 1.0f};

constexpr double MATH_PI = 3.1415926535;

class GraphicsHelper {
public:
    static glm::mat4 create_model_matrix(const Transform& transform) {
        glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), transform.position);
        glm::mat4 rot_mat = glm::mat4_cast(transform.rotation);
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), transform.scale);

        glm::mat4 model = translation_mat * rot_mat * scale_mat;

        return model;
    }
};