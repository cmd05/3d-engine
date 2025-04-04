/*
cmake --build build && ./build/3dengine.exe
cmake --build build > log.txt && ./build/3dengine.exe

cmake --build build > log.txt && ./build/3dengine.exe
*/

#include <chrono>
#include <random>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>

#include <nlohmann/json.hpp>

#include <engine/ecs/core/Scene.hpp>

#include <engine/ecs/systems/CameraControlSystem.hpp>
#include <engine/ecs/systems/PhysicsSystem.hpp>
#include <engine/ecs/systems/PlayerControlSystem.hpp>
#include <engine/ecs/systems/RenderSystem.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/Player.hpp>
#include <engine/ecs/components/Renderable.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Thrust.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>
#include <engine/ecs/components/PointLight.hpp>

#include <engine/window/WindowManager.hpp>
#include <engine/input/InputHandler.hpp>
#include <engine/gui/GUIMain.hpp>

#include <engine/config/WindowConfig.hpp>
#include <engine/config/Events.hpp>

// main scene
Scene main_scene;

static bool quit = false;

void quit_handler(Event& event) {
    quit = true;
}

void update_frame_times(float new_time, float& curr, float& last, float& dt) {
    curr = new_time;
    dt = curr - last;
    last = curr;
}

int main() {
    // -- register essential callbacks --

    // quit handler for `Application`
    main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, quit_handler));

    // initialize program state (eg: g_graphics_objects) once OpenGL has been initialized
    main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::GL_INIT, RenderSystem::gl_init_callback));

    // ----------------------------

    InputHandler input_handler {main_scene};

    // Window setup
    WindowManager window_manager {main_scene, input_handler}; // window manager requires reference to scene and input handler
    
    // GUI setup
    const char *glsl_version = "#version 130";
    GUIState gui_state;
    GUIMain gui_main {gui_state, glsl_version};

    // --- order must be exact ---
    int window_width = WindowConfig::DEFAULT_SCR_WIDTH;
    int window_height = WindowConfig::DEFAULT_SCR_HEIGHT;

    window_manager.bind_gui(gui_main);
    window_manager.init("3D Engine", window_width, window_height, 0, 0);
    
    gui_main.bind_window_manager(window_manager);
    gui_main.init("3D Engine Editor");
    
    // ---------------------------


    /// ------------- Register Components ---------------
    
    main_scene.register_component<
        Components::Camera,
        Components::Gravity,
        Components::Renderable,
        Components::RigidBody,
        Components::Transform,
        Components::Model,
        Components::Cubemap,
        Components::PointLight,
        
        Components::Player,
        Components::Thrust
    >();

    /// -------------------------------------------------

    /// ------------- Register Systems ------------------
    
    // register system to our scene and set its signature
    auto& physics_system = main_scene.register_system<PhysicsSystem>();
    auto& camera_control_system = main_scene.register_system<CameraControlSystem>(input_handler);
    auto& player_control_system = main_scene.register_system<PlayerControlSystem>();

    // create camera entity
    // primary camera to be used for rendering `main_scene`
    Entity camera_entity = main_scene.create_entity();
    int win_framebuffer_width, win_framebuffer_height;
    window_manager.get_framebuffer_size(win_framebuffer_width, win_framebuffer_height);
    ENGINE_LOG(win_framebuffer_width << " " << win_framebuffer_height);

    main_scene.add_component(
        camera_entity,
        Components::Transform {.position = glm::vec3(90.0f, 50.0f, 10.0f)}
    );

    main_scene.add_component(
        camera_entity,
        // todo: camera should be based on framebuffer dimensions not window dimensions
        Components::Camera(win_framebuffer_width, win_framebuffer_height)
    );

    Components::Camera& camera_component = main_scene.get_component<Components::Camera>(camera_entity);
    camera_component.set_cam_front(glm::vec3(-1.0f, 0.0f, 0.0f));

    auto& render_system = main_scene.register_system<RenderSystem>(camera_entity, gui_state);
    render_system.init_framebuffer_size(win_framebuffer_width, win_framebuffer_height);
    
    int models_render_count = 0;
    std::unordered_map<std::string, std::string> models;

    // parse models.json
    std::ifstream models_ifs {std::string(FS_RESOURCES_DIR) + "models.json"};
    nlohmann::json j_models;
    models_ifs >> j_models;

    if (j_models.contains("models") && j_models["models"].is_array()) {
        for (const auto& obj : j_models["models"]) {
            if (!obj.empty()) {
                auto it = obj.begin();
                std::string key = it.key();
                std::string value = it.value();
                
                models[key] = std::string(FS_RESOURCES_DIR) + value;

                models_render_count++;
            }
        }
    }

    models_interface_type models_map;
    try {
        models_map = render_system.load_models(models);
    } catch(...) {
        ASSERT_MESSAGE("Error importing models. Check debug");
    }

    // NOTE: cubemap loading will be slow since stb_image is being used each time
    // cubemaps_interface_type cubemaps = render_system.load_cubemaps({
    //     {"sky", {
    //         .right = (std::string(FS_RESOURCES_DIR) + "textures/skybox/right.jpg"),
    //         .left = (std::string(FS_RESOURCES_DIR) + "textures/skybox/left.jpg"),
    //         .top = (std::string(FS_RESOURCES_DIR) + "textures/skybox/top.jpg"),
    //         .bottom = (std::string(FS_RESOURCES_DIR) + "textures/skybox/bottom.jpg"),
    //         .front = (std::string(FS_RESOURCES_DIR) + "textures/skybox/front.jpg"),
    //         .back = (std::string(FS_RESOURCES_DIR) + "textures/skybox/back.jpg"),
    //     }},
    // });

    // /// -------------------------------------------------

    // auto cubemap_entity = main_scene.create_entity();
    // main_scene.add_component(cubemap_entity, Components::Renderable{});
    // main_scene.add_component(cubemap_entity, Components::Cubemap{.id = cubemaps.at("sky")});
    
    // Create entities

    // camera entity already exists, so we we can use one less than MAX_ENTITIES number of entities
    // std::vector<Entity> entities(MAX_ENTITIES_AFTER_CAMERA); 

    std::vector<Entity> entities(models_render_count);

    // generate random values
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    std::uniform_real_distribution<float> rand_position(-300.0f, 300.0f);
    std::uniform_real_distribution<float> rand_light_position(-70.0f, 40.0f);
    std::uniform_real_distribution<float> rand_color(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_model(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f); // gravity is negative (downwards force)

    // std::uniform_real_distribution<float> rand_scale(1.0f, 1.5f);
    std::uniform_real_distribution<float> rand_scale(3.0f, 5.0f);
    float scale = rand_scale(generator);
    
    const int NR_LIGHTS = 1;
    for(int i = 0; i < NR_LIGHTS; i++) {
        auto light_entity = main_scene.create_entity();

        main_scene.add_component(light_entity, Components::Transform{
            // .position = glm::vec3(rand_light_position(generator), 50.0f, rand_light_position(generator)),
            // .scale = glm::vec3(5.0f)
            .position = glm::vec3(10.0f, 50.0f, 10.0f),
            .scale = glm::vec3(5.0f)
        });
        
        main_scene.add_component(light_entity, Components::PointLight{
            // .light_color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            .light_color = glm::vec3(1.0f)
            // .light_color = glm::vec3(1.0f, 1.0f, 0.0f)
        });
    }

    for (auto& entity : entities)
    {
        // Create entity
        entity = main_scene.create_entity();
        
        // Add components to entity
        // main_scene.add_component<Player>(entity, Player{});

        main_scene.add_component( // automatic type deduction for template parameters
        	entity,
        	Components::Gravity {
                .force = glm::vec3(0.0f, -9.8f, 0.0f)
                // .force = glm::vec3(0.0f, rand_gravity(generator), 0.0f)
            }
        );

        main_scene.add_component(
            entity,
            Components::RigidBody {
                .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
            }
        );

        main_scene.add_component(
            entity,
            Components::Transform {
                // .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
                // .scale = glm::vec3(scale, scale, scale),
                .position = glm::vec3(0.0f),
                .scale = glm::vec3(0.2f)
            }
        );

        main_scene.add_component(
            entity,
            Components::Renderable {
                .color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            }
        );

        // select random model from `models_map`
        auto item = models_map.begin();
        std::advance( item, rand() % models_map.size() );

        main_scene.add_component(entity,
            Components::Model{.model_id = item->second }
        );
    }

    // timing information
    float last_frame, current_frame;
    last_frame = current_frame = window_manager.get_time(); // note: windowManager.get_time() is not exactly zero at this call
    float dt = 0.0f;

    render_system.set_uniforms_pre_rendering();

    // render loop
    while (!quit)
    {
        window_manager.process_events(); // first process window events

        player_control_system.update(dt);
        camera_control_system.update(dt);
        // physics_system.update(dt);
        render_system.update(dt);

        gui_main.new_frame();
        gui_main.update();
        gui_main.render();
        gui_main.update_platform_windows();

        window_manager.update(); // window updation happens after windows have been processed

        update_frame_times(window_manager.get_time(), current_frame, last_frame, dt);
    }

    // clean resources
    window_manager.shutdown();

    return 0;
}
