/*
cmake --build build && ./build/3dengine.exe
cmake --build build > log.txt && ./build/3dengine.exe

cmake --build build > log.txt && ./build/3dengine.exe
*/

#include <chrono>
#include <random>
#include <iostream>

#include <glm/glm.hpp>

#include <engine/ecs/core/Scene.hpp>

#include <engine/ecs/systems/CameraControlSystem.hpp>
#include <engine/ecs/systems/PhysicsSystem.hpp>
#include <engine/ecs/systems/PlayerControlSystem.hpp>
#include <engine/ecs/systems/RenderSystem.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/Player.hpp>
#include <engine/ecs/components/RenderableComponent.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Thrust.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>
#include <engine/ecs/components/PointLightComponent.hpp>

#include <engine/window/WindowManager.hpp>
#include <engine/input/InputHandler.hpp>

#include <engine/gui/GUIMain.hpp>

// main scene
Scene main_scene;

static bool quit = false;

void quit_handler(Event& event) {
    quit = true;
}

int main() {
    // TODO: (remove comment?) The Scene and Window class have been intentionally decoupled
    // Window stores a Scene&, and can send events to the `Scene` and "it's ecosystem i.e systems" via `Event`
    // However, instead of just events, we use InputHandler for polling. ex: keyboard keys
    
    InputHandler input_handler;

    // Window setup
    WindowManager window_manager {main_scene, input_handler}; // window manager requires reference to scene and input handler
    
    // GUI setup
    const char *glsl_version = "#version 130";
    GUIState gui_state;
    GUIMain gui_main {gui_state, glsl_version};

    // --- order must be exact ---
    window_manager.bind_gui(gui_main);
    window_manager.init("3D Engine", DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT, 0, 0);
    
    gui_main.bind_window_manager(window_manager);
    gui_main.init("3D Engine Editor");
    
    // ---------------------------

    // quit handler for `Application`
    main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, quit_handler));

    /// ------------- Register Components ---------------
    
    main_scene.register_component<
        Camera,
        Gravity,
        RenderableComponent,
        RigidBody,
        Transform,
        Model,
        Cubemap,
        PointLightComponent,
        
        Player,
        Thrust
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

    main_scene.add_component(
        camera_entity,
        Transform {.position = glm::vec3(90.0f, 50.0f, 10.0f)}
    );

    main_scene.add_component(
        camera_entity,
        Camera(DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT)
    );

    Camera& camera_component = main_scene.get_component<Camera>(camera_entity);
    camera_component.cam_front = glm::vec3(-1.0f, 0.0f, 0.0f);

    auto& render_system = main_scene.register_system<RenderSystem>(camera_entity, gui_state);
    
    const int MODELS_TO_RENDER = 1;
    // const int MODELS_TO_RENDER = 8;

    std::unordered_map<std::string, std::string> models = {
        // {"rock", std::string(FS_RESOURCES_DIR) + "models/rock/rock.obj"},
        // {"m4a1", std::string(FS_RESOURCES_DIR) +"models/m4a1/m4a1.obj"},
        // {"planet", std::string(FS_RESOURCES_DIR) +"models/planet/planet.obj"},

        // {"cyborg", std::string(FS_RESOURCES_DIR) + "models/cyborg/cyborg.obj"},
        // {"backpack", std::string(FS_RESOURCES_DIR) +"models/forest-backpack/backpack.obj"},
        // {"sponza", std::string(FS_RESOURCES_DIR) + "models/sponza-gltf/scene.gltf"},
        {"sponza-khronos", std::string(FS_RESOURCES_DIR) + "models/sponza-gltf-khronos/Sponza.gltf"},

        // {"sponza", std::string(FS_RESOURCES_DIR) + "models/sponza2/sponza.obj"},
        // {"low_poly_house", std::string(FS_RESOURCES_DIR) +"models/low_poly_house/low_poly_house.fbx"},
        // {"deccer_cubes", std::string(FS_RESOURCES_DIR) +"models/deccer-cubes/SM_Deccer_Cubes_Textured.fbx"},
        // {"bunny", std::string(FS_RESOURCES_DIR) +"models/stanford_bunny_pbr/scene.gltf"},
        // {"teapot", std::string(FS_RESOURCES_DIR) + "models/utah_teapot_pbr/scene.gltf"},
        // {"cat", std::string(FS_RESOURCES_DIR) + "models/cat/12221_Cat_v1_l3.obj"},
        // {"low_poly_tree", std::string(FS_RESOURCES_DIR) + "models/low_poly_tree/Lowpoly_tree_sample.fbx"},
        // {"nanosuit", std::string(FS_RESOURCES_DIR) +"models/nanosuit/nanosuit.obj"},
    };

    models_interface_type models_map = render_system.load_models(models);

    cubemaps_interface_type cubemaps = render_system.load_cubemaps({
        {"sky", {
            .right = (std::string(FS_RESOURCES_DIR) + "textures/skybox/right.jpg"),
            .left = (std::string(FS_RESOURCES_DIR) + "textures/skybox/left.jpg"),
            .top = (std::string(FS_RESOURCES_DIR) + "textures/skybox/top.jpg"),
            .bottom = (std::string(FS_RESOURCES_DIR) + "textures/skybox/bottom.jpg"),
            .front = (std::string(FS_RESOURCES_DIR) + "textures/skybox/front.jpg"),
            .back = (std::string(FS_RESOURCES_DIR) + "textures/skybox/back.jpg"),
        }},
    });

    /// -------------------------------------------------

    // auto cubemap_entity = main_scene.create_entity();
    // main_scene.add_component(cubemap_entity, Renderable{});
    // main_scene.add_component(cubemap_entity, Cubemap{.id = cubemaps.at("sky")});
    
    // Create entities

    // camera entity already exists, so we we can use one less than MAX_ENTITIES number of entities
    // std::vector<Entity> entities(MAX_ENTITIES_AFTER_CAMERA); 

    std::vector<Entity> entities(MODELS_TO_RENDER);

    // generate random values
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    std::uniform_real_distribution<float> rand_position(-300.0f, 300.0f);
    std::uniform_real_distribution<float> rand_light_position(-90.0f, 90.0f);
    std::uniform_real_distribution<float> rand_color(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_model(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f); // gravity is negative (downwards force)

    // std::uniform_real_distribution<float> rand_scale(1.0f, 1.5f);
    std::uniform_real_distribution<float> rand_scale(3.0f, 5.0f);
    float scale = rand_scale(generator);
    
    const int NR_LIGHTS = 1;
    for(int i = 0; i < NR_LIGHTS; i++) {
        auto light_entity = main_scene.create_entity();

        main_scene.add_component(light_entity, Transform{
            // .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
            .position = glm::vec3(10.0f, 50.0f, 10.0f),
            // .position = glm::vec3(rand_light_position(generator), 50.0f, rand_light_position(generator)),
            .scale = glm::vec3(5.0f)
        });
        
        main_scene.add_component(light_entity, PointLightComponent{
            // .light_color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            .light_color = glm::vec3(1.0f)
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
        	Gravity {
                .force = glm::vec3(0.0f, -9.8f, 0.0f)
                // .force = glm::vec3(0.0f, rand_gravity(generator), 0.0f)
            }
        );

        main_scene.add_component(
            entity,
            RigidBody {
                .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
            }
        );

        main_scene.add_component(
            entity,
            Transform {
                // .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
                // .scale = glm::vec3(scale, scale, scale),
                .position = glm::vec3(0.0f),
                .scale = glm::vec3(0.2f)
            }
        );

        main_scene.add_component(
            entity,
            RenderableComponent {
                .color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            }
        );

        // select random model from `models_map`
        auto item = models_map.begin();
        std::advance( item, rand() % models_map.size() );

        main_scene.add_component(entity,
            Model{.model_id = item->second }
        );
    }

    /// Debugging
    // ------------------------------------------------------------------------
    
    // destroy some entities
 	// auto it = entities.begin() + 1000;

    // for(int i = 0; i < 3000; i++) {
    //     main_scene.destroy_entity(*it);
    //     it = entities.erase(it);
    // }
    
    // add some entities
    // for (int i = 0; i < 2000; i++)
    // {
    //     // Create entity
    //     auto entity = main_scene.create_entity();
        
    //     // Add components to entity
    //     main_scene.add_component<Player>(entity, Player{});

    //     main_scene.add_component( // automatic type deduction for template parameters
    //     	entity,
    //     	Gravity {
    //             // .force = glm::vec3(0.0f, -9.8f, 0.0f)
    //             // .force = glm::vec3(0.0f, rand_gravity(generator), 0.0f)
    //         }
    //     );

    //     main_scene.add_component(
    //         entity,
    //         RigidBody {
    //             .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
    //             .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
    //         }
    //     );

    //     main_scene.add_component(
    //         entity,
    //         Transform {
    //             .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
    //             .scale = glm::vec3(scale, scale, scale)
    //         }
    //     );

    //     main_scene.add_component(
    //         entity,
    //         Renderable {
    //             .color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
    //         }
    //     );
    // }
   
    // ------------------------------------------------------------------------

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

        current_frame = window_manager.get_time();
        dt = current_frame - last_frame;
        last_frame = current_frame;
    }

    // clean resources
    window_manager.shutdown();

    return 0;
}
