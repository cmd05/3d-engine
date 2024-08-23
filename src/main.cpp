/*
cmake --build build && ./build/3dengine.exe
cmake --build build > log.txt && ./build/3dengine.exe

cmake --build build > log.txt && ./build/3dengine.exe
*/

#include <chrono>
#include <random>
#include <iostream>

#include <glm/glm.hpp>

#include <ecs/core/Coordinator.hpp>

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

Coordinator g_coordinator;

static bool quit = false;

void quit_handler(Event& event)
{
    quit = true;
}

int main()
{
    // initialize coordinator
    g_coordinator.init();

    // window setup
    WindowManager window_manager {g_coordinator}; // window manager requires reference to coordinator
    window_manager.init("3D engine", 1920, 1080, 0, 0);

    // quit handler
    g_coordinator.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, quit_handler));

    /// ------------- Register Components ---------------
    
    g_coordinator.register_component<
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
    
    // register system to our coordinator and set its signature
    auto& physics_system = g_coordinator.register_system<PhysicsSystem>(); // note auto& is necessary. simply auto wil create a new local variable can cause copying errors
    g_coordinator.set_system_signature<
        PhysicsSystem,
        Gravity, RigidBody, Transform
    >();
    // initialize the system
    physics_system.init();


    auto& camera_control_system = g_coordinator.register_system<CameraControlSystem>();
    g_coordinator.set_system_signature<
        CameraControlSystem,
        Camera, Transform
    >();

    camera_control_system.init();

    auto& player_control_system = g_coordinator.register_system<PlayerControlSystem>();
    g_coordinator.set_system_signature<
        PlayerControlSystem,
        Player, Transform
    >();

    player_control_system.init();

    auto& render_system = g_coordinator.register_system<RenderSystem>();
    g_coordinator.set_system_signature<
        RenderSystem,
        Renderable, Transform
    >();

    // creates a camera entity using CreateEntity()
    render_system.init();

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
        entity = g_coordinator.create_entity();
        
        // Add components to entity
        g_coordinator.add_component<Player>(entity, Player{});

        g_coordinator.add_component( // automatic type deduction for template parameters
        	entity,
        	Gravity {
                .force = glm::vec3(0.0f, -9.8f, 0.0f)
                // .force = glm::vec3(0.0f, rand_gravity(generator), 0.0f)
            }
        );

        g_coordinator.add_component(
            entity,
            RigidBody {
                .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
            }
        );

        g_coordinator.add_component(
            entity,
            Transform {
                .position = glm::vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
                .rotation = glm::vec3(rand_rotation(generator), rand_rotation(generator), rand_rotation(generator)),
                .scale = glm::vec3(scale, scale, scale)
            }
        );

        g_coordinator.add_component(
            entity,
            Renderable {
                .color = glm::vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            }
        );
    }

    /// Debugging
    /*
    // destroy some entities
 	auto it = entities.begin() + 1000;

    for(int i = 0; i < 3000; i++) {
        g_coordinator.DestroyEntity(*it);
        it = entities.erase(it);
    }
    
    // add some entities
    for (int i = 0; i < 2000; i++)
    {
        auto entity = g_coordinator.CreateEntity();
        g_coordinator.AddComponent(entity, Player{});

        g_coordinator.AddComponent<Gravity>(
            entity,
            {Vec3(0.0f, rand_gravity(generator), 0.0f)});

        g_coordinator.AddComponent(
            entity,
            RigidBody{
                .velocity = Vec3(0.0f, 0.0f, 0.0f),
                .acceleration = Vec3(0.0f, 0.0f, 0.0f)
            });

        g_coordinator.AddComponent(
            entity,
            Transform{
                .position = Vec3(rand_position(generator), rand_position(generator), rand_position(generator)),
                .rotation = Vec3(rand_rotation(generator), rand_rotation(generator), rand_rotation(generator)),
                .scale = Vec3(scale, scale, scale)
            });

        g_coordinator.AddComponent(
            entity,
            Renderable{
                .color = Vec3(rand_color(generator), rand_color(generator), rand_color(generator))
            });

        entities.push_back(entity);
    }
    */

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
