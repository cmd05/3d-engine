#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
public:
    Camera(unsigned int width, unsigned int height): view_width{width}, view_height{height} {}
public:
    float vfov = 45; // deg
    float near_clip = 0.1;
    float far_clip = 1000;
    unsigned int view_width;
    unsigned int view_height;

    glm::vec3 cam_pos = glm::vec3(0.0f);
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
};