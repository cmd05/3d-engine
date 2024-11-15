#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/components/Transform.hpp>

#include <engine/graphics/CameraWrapper.hpp>
#include <engine/graphics/Shader.hpp>

constexpr glm::vec3 MATH_X_AXIS {1.0f, 0.0f, 0.0f};
constexpr glm::vec3 MATH_Y_AXIS {0.0f, 1.0f, 0.0f};
constexpr glm::vec3 MATH_Z_AXIS {0.0f, 0.0f, 1.0f};

constexpr double MATH_PI = 3.1415926535;

class GraphicsHelper {
public:
    struct MVP {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    static glm::mat4 create_model_matrix(const Components::Transform& transform);
    static MVP get_mvp(const Components::Transform& transform, const CameraWrapper& camera_wrapper);
    // static void set_mvp(const std::unique_ptr<Shader>& shader, const Transform& transform, const CameraWrapper& camera_wrapper);
    static glm::mat4 get_mvp_matrix(const Components::Transform& transform, const CameraWrapper& camera_wrapper);
};