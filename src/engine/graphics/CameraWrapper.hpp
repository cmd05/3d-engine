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
private:
    Entity m_camera;
    Scene& ref_scene;
};