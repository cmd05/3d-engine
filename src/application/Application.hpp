#include <engine/ecs/core/Scene.hpp>
#include <engine/window/WindowManager.hpp>
#include <engine/gui/GUIMain.hpp>

// TODO: create an application class
// Application::init()
// Application::run()

class Application {
    // avoid coupled references to each other
    Scene main_scene;
    InputHandler input_handler; // not defined as a member of `Scene`
    WindowManager window_manager {main_scene, input_handler};
    GUIMain gui_layer {window_manager, "#version 130"};

    // systems
    // maybe Scene::register_system should return pointer instead of reference
    // so that we can have PhysicsSystem* m_physics_system
    
public:
    void init();
    void update();

    void run(); // run() internally calls update
};

/**

int main() {
    Application app; //
    app.init();

    app.run();
}

 */