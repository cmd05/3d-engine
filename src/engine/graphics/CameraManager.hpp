#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/core/Scene.hpp>

class CameraManager {
public:
    CameraManager(Scene& scene, Entity camera): ref_scene{scene}, m_camera{camera} {}

    void resize_view(unsigned int new_width, unsigned int new_height) {
        auto& camera = ref_scene.get_component<Camera>(m_camera);
        camera.view_width = new_width;
        camera.view_height = new_height;
    }

    glm::mat4 get_view_matrix() const { 
        const auto& camera = ref_scene.get_component<Camera>(m_camera);
        const auto& camera_transform = ref_scene.get_component<Transform>(m_camera);
        
        return glm::lookAt(camera_transform.position, camera_transform.position + camera.cam_front, camera.world_up); 
    }

    glm::mat4 get_projection_matrix() const {
        const auto& camera = ref_scene.get_component<Camera>(m_camera);
        return glm::perspective(glm::radians(camera.vfov), (float) camera.view_width / camera.view_height, camera.near_clip, camera.far_clip); 
    }
private:
    Entity m_camera;
    Scene& ref_scene;
};