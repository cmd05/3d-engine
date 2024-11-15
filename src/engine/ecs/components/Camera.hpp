#pragma once

#include <glm/glm.hpp>

#include <engine/config/GraphicsConfig.hpp>

namespace Components {

struct Camera {
public:
    Camera(unsigned int width, unsigned int height): view_width{width}, view_height{height} {
        update_cam_vectors();
        update_euler_angles();
    }

    // Note: Camera::world_up does not refer to the Scene being upwards or downwards.
    // `world_up` is what the camera is set to regard as the world's up vector.
    // For example: if world_up = glm::vec3(0, -1, 0), the camera will be show an inversed view
    // => the cam_up is always calculated based on world_up. cam_up is not to be updated by the user
    void set_world_up(glm::vec3 new_world_up) {
        world_up = glm::normalize(new_world_up);

        update_cam_vectors(); 
    }

    void set_cam_front(glm::vec3 new_cam_front) {
        cam_front = glm::normalize(new_cam_front);
        
        update_cam_vectors();
        update_euler_angles();
    }
public:
    // Note: 
    // camera rotation is done using euler angles not Transform::rotation
    // camera position is handled using Transform::position

    // rendering parameters
    float vfov = GraphicsConfig::Camera::DEFAULT_CAM_VFOV;
    float near_clip = GraphicsConfig::Camera::DEFAULT_CAM_NEAR_CLIP;
    float far_clip = GraphicsConfig::Camera::DEFAULT_CAM_FAR_CLIP;
    
    unsigned int view_width;
    unsigned int view_height;

    // Camera direction vectors
    glm::vec3 world_up = GraphicsConfig::Camera::DEFAULT_CAM_WORLD_UP;
    glm::vec3 cam_front = GraphicsConfig::Camera::DEFAULT_CAM_FRONT;
    glm::vec3 cam_right;
    glm::vec3 cam_up;

    // Camera rotation
    float yaw, pitch, roll;

private:
    void update_euler_angles() { 
        yaw = atan2(cam_front.z, cam_front.x);
        pitch = glm::asin(cam_front.y);
        roll = atan2(cam_up.z, cam_up.y);
    }

    void update_cam_vectors() {
        // if either cam_front or world_up are changed, we must
        // update cam_right and cam_up vectors
        cam_right = glm::normalize(glm::cross(cam_front, world_up));
        cam_up = glm::normalize(glm::cross(cam_right, cam_front));
    }
};

};