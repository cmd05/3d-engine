#pragma once

#include <glm/glm.hpp>

#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/core/Scene.hpp>

#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Transform.hpp>

class CameraWrapper {
public:
    CameraWrapper(Scene& scene, Entity camera);

    void resize_view(unsigned int new_width, unsigned int new_height);

    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;

    void translate_camera(BasicMovement direction, float distance);
    void rotate_camera(double x_offset, double y_offset);
    void zoom_camera(double offset);

    const Transform& get_transform_component() const;
    const Camera& get_camera_component() const;
private:
    Entity m_camera;
    Scene* m_scene;
};