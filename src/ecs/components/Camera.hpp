#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

const double PI = 3.141592653589793;

struct Camera
{
    glm::mat4 projectionTransform;

    static inline glm::mat4 MakeProjectionTransform(float fov, float nearClip, float farClip, unsigned int viewWidth, unsigned int viewHeight);
};

inline glm::mat4 Camera::MakeProjectionTransform(float fov, float nearClip, float farClip, unsigned int viewWidth, unsigned int viewHeight)
{
    /// Old version with math lib used aspect ratio on Y instead of X:
    // ```
    // float zoomX = 1.0f / tanf((fov / 2.0f) * (M_PI / 180.0f));
    // float zoomY = (zoomX * static_cast<float>(viewWidth)) / static_cast<float>(viewHeight);
    // Mat44 transform;
    // transform.m[0][0] = zoomX;
    // transform.m[1][1] = zoomY;
    // ```
    
    return glm::perspective(glm::radians(fov), (float) viewWidth / viewHeight, nearClip, farClip);
}