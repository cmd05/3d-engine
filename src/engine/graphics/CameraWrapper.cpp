#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/CameraWrapper.hpp>
#include <engine/graphics/lib/GraphicsHelper.hpp>

#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Transform.hpp>

#include <engine/ecs/configs/CameraConfigs.hpp>

CameraWrapper::CameraWrapper(Scene& scene, Entity camera): 
    ref_scene{scene}, m_camera{camera} {}

void CameraWrapper::resize_view(unsigned int new_width, unsigned int new_height) {
    auto& camera = ref_scene.get_component<Camera>(m_camera);
    camera.view_width = new_width;
    camera.view_height = new_height;
}

glm::mat4 CameraWrapper::get_view_matrix() const { 
    // Using camera quaternion
    // glm::mat4 mat_rot = glm::transpose(glm::mat4_cast(camera_transform.qrotation));
    // glm::mat4 mat_translation = glm::translate(glm::mat4(1.0f), -camera_transform.position);
    // glm::mat4 view_matrix = mat_rot * mat_translation;
    
    // return view_matrix;

    const auto& camera_transform = ref_scene.get_component<Transform>(m_camera);
    auto& camera = ref_scene.get_component<Camera>(m_camera);
    
    return glm::lookAt(camera_transform.position, camera_transform.position + camera.cam_front, camera.cam_up); 
    
}

glm::mat4 CameraWrapper::get_projection_matrix() const {
    const auto& camera = ref_scene.get_component<Camera>(m_camera);
    return glm::perspective(camera.vfov, (float) camera.view_width / camera.view_height, camera.near_clip, camera.far_clip); 
}

void CameraWrapper::rotate_camera(double x_offset, double y_offset) {
    auto& camera = ref_scene.get_component<Camera>(m_camera);

    // apply offsets
    camera.yaw += glm::radians(x_offset);
    camera.pitch += glm::radians(y_offset);

    // clamp pitch to pitch limits
    camera.pitch = glm::clamp(camera.pitch, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);

    camera.cam_front.x = cos(camera.yaw) * cos(camera.pitch);
    camera.cam_front.y = sin(camera.pitch);
    camera.cam_front.z = sin(camera.yaw) * cos(camera.pitch);
    camera.cam_front = glm::normalize(camera.cam_front);

    camera.cam_right = glm::normalize(glm::cross(camera.cam_front, camera.world_up));
    camera.cam_up    = glm::normalize(glm::cross(camera.cam_right, camera.cam_front));
    
    // update camera quaternion
    // euler to quat rotations
    // glm::quat q_yaw = glm::angleAxis(camera.yaw, MATH_Y_AXIS);
    // glm::quat q_pitch = glm::angleAxis(camera.pitch, MATH_X_AXIS);
    
    // // update quaternion
    // camera_transform.qrotation = q_yaw * q_pitch;
    // camera_transform.qrotation = glm::normalize(camera_transform.qrotation);
}

void CameraWrapper::translate_camera(BasicMovement direction, float distance) {
    auto& camera_transform = ref_scene.get_component<Transform>(m_camera);
    auto& camera = ref_scene.get_component<Camera>(m_camera);

    if (direction == BasicMovement::Forward)
        camera_transform.position += camera.cam_front * distance;
    else if (direction == BasicMovement::Backward)
        camera_transform.position -= camera.cam_front * distance;
    if (direction == BasicMovement::Right)
        camera_transform.position += camera.cam_right * distance;
    else if (direction == BasicMovement::Left)
        camera_transform.position -= camera.cam_right * distance;
    if (direction == BasicMovement::Up)
        camera_transform.position += camera.world_up * distance;
    else if (direction == BasicMovement::Down)
        camera_transform.position -= camera.world_up * distance;
}

void CameraWrapper::zoom_camera(double offset) {
    auto& camera = ref_scene.get_component<Camera>(m_camera);
    camera.vfov -= (float) glm::radians(offset);
    camera.vfov = glm::clamp(camera.vfov, CAMERA_VFOV_MIN, CAMERA_VFOV_MAX);
}