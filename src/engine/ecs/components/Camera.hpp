#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::mat4 projection_transform;
    glm::mat4 view_matrix;

    static inline glm::mat4 create_projection_transform(float fov, float near_clip, float far_clip, unsigned int view_width, unsigned int view_height);
    static inline glm::mat4 create_view_matrix(glm::vec3 cam_pos, glm::vec3 cam_front, glm::vec3 up);
};

inline glm::mat4 Camera::create_projection_transform(float fov, float near_clip, float far_clip, unsigned int view_width, unsigned int view_height) {
    /// Old version with math lib used aspect ratio on Y instead of X:
    // ```
    // float zoomX = 1.0f / tanf((fov / 2.0f) * (M_PI / 180.0f));
    // float zoomY = (zoomX * static_cast<float>(view_width)) / static_cast<float>(view_height);
    // Mat44 transform;
    // transform.m[0][0] = zoomX;
    // transform.m[1][1] = zoomY;
    // ```
    
    return glm::perspective(glm::radians(fov), (float) view_width / view_height, near_clip, far_clip);
}

inline glm::mat4 Camera::create_view_matrix (
    glm::vec3 cam_pos = glm::vec3(0.0f),
    glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
) {
    return glm::lookAt(cam_pos, cam_pos + cam_front, up);
}