#pragma once

#include <glm/glm.hpp>

class GUIState {
public:
    // ---------- BLINN PHONG SETTINGS -----------

    // lighting strengths (same for all lights)
    float ambient_strength = 0.05;
    float diffuse_strength;
    float specular_strength;
    float specular_highlight = 32.0f;

    // attenuation
    bool attenuation = false;
    float att_const = 1.0f;
    float att_linear = 0.09;
    float att_quadratic = 0.032;

    // glm::vec3 camera_pos;
    // bool bloom = false;
    bool normal_mapping = true;

    bool gamma_correction = true;

    int light0_pos[3] = {10, 50, 10};

    int scene_num; // (1) sponza; (2) cyborg, backpack; 
    int num_point_lights;

    // -------------------------------------------

    // PBR Settings

    // hdr
    float exposure = 1.5;
    bool hdr_enabled = true;
};