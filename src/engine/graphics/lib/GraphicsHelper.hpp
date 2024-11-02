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
    static glm::mat4 create_model_matrix(const Transform& transform) {
        glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), transform.position);
        glm::mat4 rot_mat = glm::mat4_cast(transform.rotation);
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), transform.scale);

        glm::mat4 model = translation_mat * rot_mat * scale_mat;

        return model;
    }

    struct MVP {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    static MVP get_mvp(const Transform& transform, const CameraWrapper& camera_wrapper) {
        MVP mvp;

        mvp.model = create_model_matrix(transform);
        mvp.view = camera_wrapper.get_view_matrix();
        mvp.projection = camera_wrapper.get_projection_matrix();

        return mvp;
    }

    static void set_mvp(const std::unique_ptr<Shader>& shader, const Transform& transform, const CameraWrapper& camera_wrapper) {
        MVP mvp = get_mvp(transform, camera_wrapper);
        set_mvp(shader, mvp);
    }

    static void set_mvp(const std::unique_ptr<Shader>& shader, MVP mvp, 
        const std::string model_uniform = "model", const std::string view_uniform = "view", const std::string projection_uniform = "projection") {
        shader->activate();

        shader->set_uniform<glm::mat4>(model_uniform, mvp.model);
        shader->set_uniform<glm::mat4>(view_uniform, mvp.view);
        shader->set_uniform<glm::mat4>(projection_uniform, mvp.projection);
    }
};