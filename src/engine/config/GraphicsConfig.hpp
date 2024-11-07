#pragma once

#include <glm/glm.hpp>

struct GraphicsConfig {
    struct Camera { 
        // camera projection values
        static constexpr float DEFAULT_CAM_VFOV = glm::radians(45.0f);
        static constexpr float DEFAULT_CAM_NEAR_CLIP = 0.1;
        static constexpr float DEFAULT_CAM_FAR_CLIP = 1000;

        // camera direction vectors
        static constexpr glm::vec3 DEFAULT_CAM_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
        static constexpr glm::vec3 DEFAULT_CAM_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
        static constexpr glm::vec3 DEFAULT_CAM_UP = DEFAULT_CAM_WORLD_UP;

        // camera movement
        static constexpr float CAMERA_PITCH_LIMIT = glm::radians(89.0f);
        static constexpr float CAMERA_VFOV_MIN = glm::radians(1.0f);
        static constexpr float CAMERA_VFOV_MAX = glm::radians(45.0f);

        // camera input
        static constexpr double CAMERA_MOUSE_SENSITIVITY = 0.05;
        static constexpr double CAMERA_SCROLL_SENSITIVITY = 1.0;
        static constexpr float CAMERA_SPEED = 90.0f;

        // camera position (stored in `Transform`)
        static constexpr glm::vec3 DEFAULT_CAM_POS = glm::vec3(0.0f);
    };
};