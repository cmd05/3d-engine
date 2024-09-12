#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/components/Transform.hpp>

class GraphicsHelper {
public:
    static glm::mat4 create_model_matrix(const Transform& transform) {
        glm::mat4 model = glm::mat4(1.0);

        model = glm::translate(model, transform.position);
        model = glm::rotate(model, (transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform.scale);

        return model;
    }
};