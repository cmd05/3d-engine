#pragma once

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/System.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/window/WindowManager.hpp>

class CameraControlSystem : public System {
public:
    CameraControlSystem(Scene& scene);
    void update(float dt);

private:
    struct CameraRotateData {
        bool b_rotate = false;
        WindowManager::MouseData rotation;
    } m_camera_rotation;
    
    struct CameraZoomData {
        bool b_zoom = false;
        double zoom_offset;
    } m_camera_zoom;

    void mouse_listener(Event& event);
    void scroll_listener(Event& event);
};