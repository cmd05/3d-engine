#include <engine/ecs/systems/CameraControlSystem.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>
#include <engine/ecs/core/SceneView.hpp>

#include <engine/window/WindowManager.hpp>
#include <engine/graphics/CameraWrapper.hpp>

#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Camera.hpp>

#include <engine/config/GraphicsConfig.hpp>

CameraControlSystem::CameraControlSystem(Scene& scene, InputHandler& input_handler): System(scene) {
    m_input_handler = &input_handler;

    ref_scene.add_event_listener(METHOD_LISTENER(Events::Input::MOUSE, CameraControlSystem::mouse_listener));
    ref_scene.add_event_listener(METHOD_LISTENER(Events::Input::SCROLL, CameraControlSystem::scroll_listener));
}

void CameraControlSystem::update(float dt) {
    if(!WindowManager::is_window_focused())
        return;

    for (Entity entity : SceneView<Camera, Transform>(ref_scene)) {
        CameraWrapper camera_wrapper{ref_scene, entity};
        float cam_offset = dt * GraphicsConfig::Camera::CAMERA_SPEED;

        // poll camera keys
        // TODO: move these keys to a common key mapping .cpp file - KeyMap.cpp
        if(m_input_handler->get_key(GLFW_KEY_W))
            // no need to reset_key here
            camera_wrapper.translate_camera(BasicMovement::Forward, cam_offset);
        else if(m_input_handler->get_key(GLFW_KEY_S))
            camera_wrapper.translate_camera(BasicMovement::Backward, cam_offset);
        if(m_input_handler->get_key(GLFW_KEY_A))
            camera_wrapper.translate_camera(BasicMovement::Left, cam_offset);
        else if(m_input_handler->get_key(GLFW_KEY_D))
            camera_wrapper.translate_camera(BasicMovement::Right, cam_offset);
        if(m_input_handler->get_key(GLFW_KEY_Q))
            camera_wrapper.translate_camera(BasicMovement::Up, cam_offset);
        else if(m_input_handler->get_key(GLFW_KEY_E))
            camera_wrapper.translate_camera(BasicMovement::Down, cam_offset);

        // TODO:
        // m_input_handler->react_key_noprocess(GLFW_KEY_W, []() {})

        // rotate camera
        if(m_camera_rotation.b_rotate) {
            camera_wrapper.rotate_camera(m_camera_rotation.rotation.x_offset, m_camera_rotation.rotation.y_offset);
            m_camera_rotation.b_rotate = false;
        }

        // zoom camera
        if(m_camera_zoom.b_zoom) {
            camera_wrapper.zoom_camera(m_camera_zoom.zoom_offset);
            m_camera_zoom.b_zoom = false;
        }
    }
}

void CameraControlSystem::mouse_listener(Event& event) {
    m_camera_rotation.rotation = event.get_param<InputHandler::MouseData>(Events::Input::Mouse::MOUSE_DATA);
    m_camera_rotation.rotation.x_offset *= GraphicsConfig::Camera::CAMERA_MOUSE_SENSITIVITY;
    m_camera_rotation.rotation.y_offset *= GraphicsConfig::Camera::CAMERA_MOUSE_SENSITIVITY;

    m_camera_rotation.b_rotate = true;
}

void CameraControlSystem::scroll_listener(Event& event) {
    InputHandler::ScrollData scroll_data = event.get_param<InputHandler::ScrollData>(Events::Input::Scroll::SCROLL_DATA);
    m_camera_zoom.zoom_offset = scroll_data.y_offset * GraphicsConfig::Camera::CAMERA_SCROLL_SENSITIVITY;
    
    m_camera_zoom.b_zoom = true;
}