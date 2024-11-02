#pragma once

#include <glm/glm.hpp>

class GUIState {
public:
    // lighting strenghts (same for all lights)
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
    float specular_highlight = 32.0f;

    // attenuation
    float att_const = 1.0f;
    float att_linear = 0.09;
    float att_quadratic = 0.032;

    // glm::vec3 camera_pos;
    // bool bloom = false;
    bool normal_mapping = true;

    bool gamma_correction = true;
};