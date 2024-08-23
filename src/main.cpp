/*
cmake --build build && ./build/3dengine.exe
cmake --build build > log.txt && ./build/3dengine.exe

cmake --build build > log.txt && ./build/3dengine.exe
*/

#include <chrono>
#include <random>
#include <iostream>

#include <glm/glm.hpp>

#include <ecs/core/Scene.hpp>

#include <ecs/systems/CameraControlSystem.hpp>
#include <ecs/systems/PhysicsSystem.hpp>
#include <ecs/systems/PlayerControlSystem.hpp>
#include <ecs/systems/RenderSystem.hpp>

#include <ecs/components/Camera.hpp>
#include <ecs/components/Gravity.hpp>
#include <ecs/components/Player.hpp>
#include <ecs/components/Renderable.hpp>
#include <ecs/components/RigidBody.hpp>
#include <ecs/components/Thrust.hpp>
#include <ecs/components/Transform.hpp>

#include <window/WindowManager.hpp>

Scene main_scene;

static bool quit = false;

void quit_handler(Event& event)
{
    quit = true;
}

int main()
{
    // initialize scene
    main_scene.init();

    // window setup
    WindowManager window_manager {main_scene}; // window manager requires reference to scene
    window_manager.init("3D engine", 1920, 1080, 0, 0);

    // quit handler
    main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, quit_handler));

    /// ------------- Register Components ---------------
    
    main_scene.register_component<
        Camera,
        Gravity,
        Renderable,
        RigidBody,
        Transform,
        
        Player,
        Thrust
    >();

    /// -------------------------------------------------

    /// ------------- Register Systems ------------------
    
    // register system to our scene and set its signature
    auto& physics_system = main_scene.register_system<PhysicsSystem>(); // note auto& is necessary. simply auto wil create a new local variable can cause copying errors
    physics_system.init();

    auto& camera_control_system = main_scene.register_system<CameraControlSystem>();
    camera_control_system.init();

    auto& player_control_system = main_scene.register_system<PlayerControlSystem>();
    player_control_system.init();

    auto& render_system = main_scene.register_system<RenderSystem>();
    render_system.init(); // render_system::init() creates a camera entity

    /// -------------------------------------------------

    // Create entities

    // camera entity already exists, so we we can use one less than MAX_ENTITIES number of entities
    std::vector<Entity> entities(MAX_ENTITIES_AFTER_CAMERA); 

    // generate random values
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand_position(-100.0f, 100.0f);
    std::uniform_real_distribution<float> rand_rotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> rand_scale(3.0f, 5.0f);
    std::uniform_real_distribution<float> rand_color(0.0f, 1.0f);
    std::uniform_real_distribution<float> rand_gravity(-10.0f, -1.0f); // gravity is negative (downwards force)

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
                .rotation = glm::vec3(rand_rotation(generator), rand_rotation(generator), rand_rotation(generator)),
                .scale = glm::vec3(scale, scale, scale)
            }
        );

        main_scene.add_component(
            entity,
            Renderable {
                .color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            }
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
    //             .rotation = glm::vec3(rand_rotation(generator), rand_rotation(generator), rand_rotation(generator)),
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
        window_manager.process_events();

        player_control_system.update(dt);

        camera_control_system.update(dt);

        physics_system.update(dt);

        render_system.update(dt);

        window_manager.update();

        current_frame = window_manager.get_time();
        dt = current_frame - last_frame;
        last_frame = current_frame;
    }

    // clean resources
    window_manager.shutdown();

    return 0;
}
