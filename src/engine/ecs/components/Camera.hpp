#pragma once

#include <glm/glm.hpp>

struct Camera {
public:
    Camera(unsigned int width, unsigned int height): view_width{width}, view_height{height} {}
public:
    // rendering parameters
    // TODO: Move all camera config values from CameraControlSystem, Camera.hpp, CameraWrapper to config file / Types.hpp
    float vfov = glm::radians(45.0f);
    float near_clip = 0.1;
    float far_clip = 1000;
    unsigned int view_width;
    unsigned int view_height;

    // camera direction vectors
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cam_pos = glm::vec3(0.0f);
    glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_up = world_up;
    glm::vec3 cam_right = glm::normalize(glm::cross(cam_front, world_up));

    // camera rotation
    // Note: Currently we are not updating and using Transform::rotation (quat) for camera entities.
    // Camera rotation is done using euler angles
    float yaw = glm::asin(cam_front.z); // default yaw corresponding to cam_front
    float pitch = 0;
};