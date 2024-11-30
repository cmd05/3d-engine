#pragma once

#include <glm/glm.hpp>

struct ShaderDataTypes {
    struct MeshMatrices {
        glm::mat4 model_matrix;
        glm::mat4 mvp_matrix;
        glm::mat3 normal_matrix;
    };

    struct PointLight {
        glm::vec4 position;
        glm::vec4 color;
    };

    struct MeshProperties {
        int ambient_tex_exists = 0;
        int diffuse_tex_exists = 0;
        int specular_tex_exists = 0;
        int normal_tex_exists = 0;
    };
};