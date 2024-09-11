#include <engine/graphics/CameraManager.hpp>

CameraManager::CameraManager(Scene& scene, Entity camera): 
    ref_scene{scene}, m_camera{camera} {}

void CameraManager::resize_view(unsigned int new_width, unsigned int new_height) {
    auto& camera = ref_scene.get_component<Camera>(m_camera);
    camera.view_width = new_width;
    camera.view_height = new_height;
}

glm::mat4 CameraManager::get_view_matrix() const { 
    const auto& camera = ref_scene.get_component<Camera>(m_camera);
    const auto& camera_transform = ref_scene.get_component<Transform>(m_camera);
    
    return glm::lookAt(camera_transform.position, camera_transform.position + camera.cam_front, camera.world_up); 
}

glm::mat4 CameraManager::get_projection_matrix() const {
    const auto& camera = ref_scene.get_component<Camera>(m_camera);
    return glm::perspective(glm::radians(camera.vfov), (float) camera.view_width / camera.view_height, camera.near_clip, camera.far_clip); 
}