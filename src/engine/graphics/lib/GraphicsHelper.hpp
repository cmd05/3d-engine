#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/components/Transform.hpp>

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