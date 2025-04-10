#include <engine/ecs/core/Scene.hpp>
#include <engine/window/WindowManager.hpp>
#include <engine/gui/GUIMain.hpp>

#include <engine/ecs/systems/CameraControlSystem.hpp>
#include <engine/ecs/systems/PhysicsSystem.hpp>
#include <engine/ecs/systems/PlayerControlSystem.hpp>
#include <engine/ecs/systems/RenderSystem.hpp>

#include <memory>
// Application::init()
// Application::run()

class Application {
public:
    Application();
    void init();

    void load_models();
    void init_lights();
    void init_models();

    // systems
    // maybe Scene::register_system should return pointer instead of reference
    // so that we can have PhysicsSystem* m_physics_system
    
    // void init();
    void update();
    
    void run(); // runs till window is not closed
private:
    void register_callbacks();
    void register_ecs_components();
    void init_camera(int width, int height);
    void register_ecs_systems(); // automatically creates main camera entity

    void update_frame_times(float new_time, float& curr_time, float& last_time, float& dt);
    void quit_handler(Event& event);
private:
    Scene m_main_scene;
    std::unique_ptr<InputHandler> m_input_handler;
    std::unique_ptr<WindowManager> m_window_manager;
    std::unique_ptr<GUIMain> m_gui_main;
    std::unique_ptr<GUIState> m_gui_state;

    // Scene owns the systems
    PhysicsSystem* m_physics_system;
    CameraControlSystem* m_camera_control_system;
    RenderSystem* m_render_system;

    models_interface_type m_models_map;
    
    Entity m_camera_entity; // primary camera to be used for rendering the main scene

    const char* m_glsl_version = "#version 130";
    bool m_quit = false;
};
