#pragma once

#include <glm/glm.hpp>

#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/core/Scene.hpp>

class CameraWrapper {
public:
    CameraWrapper(Scene& scene, Entity camera);

    void resize_view(unsigned int new_width, unsigned int new_height);
    
    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;

    void translate_camera(BasicMovement direction, float distance);
    void rotate_camera(double x_offset, double y_offset);
    void zoom_camera(double offset);
private:
    // TODO: perhaps we can have a `Camera&` and `Transform&` instead of retrieving them each time
    // to consider: if a camera entity uses a different Transform component in memory (unlikely occurence) 
    Entity m_camera;

    Scene& ref_scene;
};