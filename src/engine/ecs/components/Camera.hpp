#pragma once

#include <glm/glm.hpp>

#include <engine/config/GraphicsConfig.hpp>

struct Camera {
public:
    Camera(unsigned int width, unsigned int height): view_width{width}, view_height{height} {}
public:
    // rendering parameters
    float vfov = GraphicsConfig::Camera::DEFAULT_CAM_VFOV;
    float near_clip = GraphicsConfig::Camera::DEFAULT_CAM_NEAR_CLIP;
    float far_clip = GraphicsConfig::Camera::DEFAULT_CAM_FAR_CLIP;
    unsigned int view_width;
    unsigned int view_height;

    // camera direction vectors
    glm::vec3 world_up = GraphicsConfig::Camera::DEFAULT_CAM_WORLD_UP;
    glm::vec3 cam_pos = GraphicsConfig::Camera::DEFAULT_CAM_POS;
    glm::vec3 cam_front = GraphicsConfig::Camera::DEFAULT_CAM_FRONT;
    glm::vec3 cam_up = world_up;
    glm::vec3 cam_right = glm::normalize(glm::cross(cam_front, world_up));

    // camera rotation
    // Note: Currently we are not updating and using Transform::rotation (quat) for camera entities.
    // Camera rotation is done using euler angles
    float yaw = GraphicsConfig::Camera::DEFAULT_CAM_YAW; // default yaw corresponding to cam_front
    float pitch = GraphicsConfig::Camera::DEFAULT_CAM_PITCH;
};