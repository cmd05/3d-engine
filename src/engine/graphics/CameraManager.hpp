#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/core/Scene.hpp>

class CameraManager {
public:
    CameraManager(Scene& scene, Entity camera);

    void resize_view(unsigned int new_width, unsigned int new_height);
    
    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;
private:
    Entity m_camera;
    Scene& ref_scene;
};