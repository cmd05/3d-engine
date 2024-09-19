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
#include <engine/ecs/components/Renderable.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Thrust.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>

#include <engine/window/WindowManager.hpp>

// main scene
Scene main_scene;

static bool quit = false;

void quit_handler(Event& event) {
    quit = true;
}

int main() {
    // window setup
    WindowManager window_manager {main_scene, main_scene.input_handler}; // window manager requires reference to scene and input handler
    window_manager.init("3D engine", DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT, 0, 0);

    // quit handler for `Application`
    main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, quit_handler));

    /// ------------- Register Components ---------------
    
    main_scene.register_component<
        Camera,
        Gravity,
        Renderable,
        RigidBody,
        Transform,
        Model,
        Cubemap,
        
        Player,
        Thrust
    >();

    /// -------------------------------------------------

    /// ------------- Register Systems ------------------
    
    // register system to our scene and set its signature
    // auto& physics_system = main_scene.register_system<PhysicsSystem>();
    auto& camera_control_system = main_scene.register_system<CameraControlSystem>();
    auto& player_control_system = main_scene.register_system<PlayerControlSystem>();

    // create camera entity
    // primary camera to be used for rendering `main_scene`
    Entity camera_entity = main_scene.create_entity();

    main_scene.add_component(
        camera_entity,
        Transform {.position = glm::vec3(0.0f, 0.0f, 500.0f)}
    );

    main_scene.add_component(
        camera_entity,
        Camera(DEFAULT_SCR_WIDTH, DEFAULT_SCR_HEIGHT)
    );

    auto& render_system = main_scene.register_system<RenderSystem>(camera_entity);
    
    std::unordered_map<std::string, std::string> models = {
        {"rock", std::string(FS_RESOURCES_DIR) + "models/rock/rock.obj"},
        {"cyborg", std::string(FS_RESOURCES_DIR) + "models/cyborg/cyborg.obj"},
        {"m4a1", std::string(FS_RESOURCES_DIR) +"models/m4a1/m4a1.obj"},
        {"planet", std::string(FS_RESOURCES_DIR) +"models/planet/planet.obj"},
        // {"backpack", std::string(FS_RESOURCES_DIR) +"models/forest-backpack/backpack.obj"},
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

    auto cubemap_entity = main_scene.create_entity();
    main_scene.add_component(cubemap_entity, Renderable{});
    main_scene.add_component(cubemap_entity, Cubemap{.id = cubemaps.at("sky")});
    
    // Create entities

    // camera entity already exists, so we we can use one less than MAX_ENTITIES number of entities
    // std::vector<Entity> entities(MAX_ENTITIES_AFTER_CAMERA); 
    std::vector<Entity> entities(10);

    // generate random values
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand_position(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rand_color(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_model(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f); // gravity is negative (downwards force)

    // std::uniform_real_distribution<float> rand_scale(1.0f, 1.5f);
    std::uniform_real_distribution<float> rand_scale(3.0f, 5.0f);
    float scale = rand_scale(generator);

    for (auto& entity : entities)
    {
        // Create entity
        entity = main_scene.create_entity();
        
        // Add components to entity
        main_scene.add_component<Player>(entity, Player{});

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
                .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
                .scale = glm::vec3(scale, scale, scale)
                // .scale = glm::vec3(0.02f)
            }
        );

        main_scene.add_component(
            entity,
            Renderable {
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

    // render loop
    while (!quit)
    {
        window_manager.process_events(); // first process window events

        player_control_system.update(dt);
        camera_control_system.update(dt);
        // physics_system.update(dt);
        render_system.update(dt);

        window_manager.update(); // window updation happens after windows have been processed

        current_frame = window_manager.get_time();
        dt = current_frame - last_frame;
        last_frame = current_frame;
    }

    // clean resources
    window_manager.shutdown();

    return 0;
}
