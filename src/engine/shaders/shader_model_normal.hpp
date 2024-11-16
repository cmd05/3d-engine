#pragma once

#include <glm/glm.hpp>

struct ShaderNormal {
    static const int NUM_LIGHTS = 1;

    struct Light {
        glm::vec3 position;
        glm::vec3 color;
    };

    struct ub_matrices {
        glm::mat4 model;
        glm::mat4 mvp_matrix;
        glm::mat3 normal_matrix;
    };

    struct ub_camera {
        glm::vec3 view_pos;
    };

    struct ub_lights {
        Light point_lights[NUM_LIGHTS];
    };

    struct ub_texture_params {
        int ambient_tex_exists;
        int diffuse_tex_exists;
        int specular_tex_exists;
        int u_normal_map_exists;
    };

    struct ub_light_params {
        float ambient_strength;
    };
};