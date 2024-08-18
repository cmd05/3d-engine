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

Coordinator gCoordinator;

static bool quit = false;

void QuitHandler(Event& event)
{
    quit = true;
}

int main()
{
    gCoordinator.init();


    WindowManager windowManager;
    windowManager.Init("3D engine", 1920, 1080, 0, 0);


    gCoordinator.add_event_listener(FUNCTION_LISTENER(Events::Window::QUIT, QuitHandler));


    // gCoordinator.register_component<Camera>();
    // gCoordinator.register_component<Gravity>();
    // gCoordinator.register_component<Player>();
    // gCoordinator.register_component<Renderable>();
    // gCoordinator.register_component<RigidBody>();
    // gCoordinator.register_component<Thrust>();
    // gCoordinator.register_component<Transform>();

    gCoordinator.register_component<
        Camera,
        Gravity,
        Player,
        Renderable,
        RigidBody,
        Thrust,
        Transform
    >();

    // register system to our coordinator and set its signature
    auto& physicsSystem = gCoordinator.register_system<PhysicsSystem>(); // note auto& is necessary. simply auto wil cause a new stack variable can cause copying errors for us
    // {
        // Signature signature;
        // signature.set(gCoordinator.get_component_type<Gravity>());
        // signature.set(gCoordinator.get_component_type<RigidBody>());
        // signature.set(gCoordinator.get_component_type<Transform>());
        
        // gCoordinator.set_system_signature<PhysicsSystem>(signature);
        gCoordinator.set_system_signature<
            PhysicsSystem,
            Gravity, RigidBody, Transform
        >();
    // }

    // initialize the system
    // physicsSystem.Init(gCoordinator);
    physicsSystem.Init();


    auto& cameraControlSystem = gCoordinator.register_system<CameraControlSystem>();
    // {
        // Signature signature;
        // signature.set(gCoordinator.get_component_type<Camera>());
        // signature.set(gCoordinator.get_component_type<Transform>());
        // gCoordinator.set_system_signature<CameraControlSystem>(signature);
        gCoordinator.set_system_signature<
            CameraControlSystem,
            Camera, Transform
        >();
    // }
    
    cameraControlSystem.init();


    auto& playerControlSystem = gCoordinator.register_system<PlayerControlSystem>();
    // {
    //     Signature signature;
    //     signature.set(gCoordinator.get_component_type<Player>());
    //     signature.set(gCoordinator.get_component_type<Transform>());
    //     gCoordinator.set_system_signature<PlayerControlSystem>(signature);
        gCoordinator.set_system_signature<
            PlayerControlSystem,
            Player, Transform
        >();
    // }

    playerControlSystem.Init();


    auto& renderSystem = gCoordinator.register_system<RenderSystem>();
    // {
    //     Signature signature;
    //     signature.set(gCoordinator.get_component_type<Renderable>());
    //     signature.set(gCoordinator.get_component_type<Transform>());
        
        gCoordinator.set_system_signature<
            RenderSystem,
            Renderable, Transform
        >();
    // }

    // creates camera entity using CreateEntity()
    renderSystem.Init();

    // camera entity already exists, so we we can use one less than MAX_ENTITIES number of entities
    std::vector<Entity> entities(MAX_ENTITIES_AFTER_CAMERA); 

    std::default_random_engine generator;
    std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
    std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
    std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
    std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

    float scale = randScale(generator);

    // int i = 0;
    for (auto& entity : entities)
    {
        // std::cout << i << '\n';
        entity = gCoordinator.create_entity();
        // i++;
        gCoordinator.add_component<Player>(entity, Player{});

        // gCoordinator.add_component<Gravity>(
        // 	entity,
        // 	{glm::vec3(0.0f, randGravity(generator), 0.0f)});

        gCoordinator.add_component( // automatic type deduction for template parameters
            entity,
            RigidBody{
                .velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
            });

        gCoordinator.add_component(
            entity,
            Transform {
                .position = glm::vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
                .rotation = glm::vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
                .scale = glm::vec3(scale, scale, scale)
            }
        );

        gCoordinator.add_component(
            entity,
            Renderable{
                .color = glm::vec3(randColor(generator), randColor(generator), randColor(generator))
            });
    }

    
    // destroy some entities
/* 	auto it = entities.begin() + 1000;

    for(int i = 0; i < 3000; i++) {
        gCoordinator.DestroyEntity(*it);
        it = entities.erase(it);
    }
    
    // add some entities
    for (int i = 0; i < 2000; i++)
    {
        auto entity = gCoordinator.CreateEntity();
        gCoordinator.AddComponent(entity, Player{});

        gCoordinator.AddComponent<Gravity>(
            entity,
            {Vec3(0.0f, randGravity(generator), 0.0f)});

        gCoordinator.AddComponent(
            entity,
            RigidBody{
                .velocity = Vec3(0.0f, 0.0f, 0.0f),
                .acceleration = Vec3(0.0f, 0.0f, 0.0f)
            });

        gCoordinator.AddComponent(
            entity,
            Transform{
                .position = Vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
                .rotation = Vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
                .scale = Vec3(scale, scale, scale)
            });

        gCoordinator.AddComponent(
            entity,
            Renderable{
                .color = Vec3(randColor(generator), randColor(generator), randColor(generator))
            });

        entities.push_back(entity);
    } */

    float lastFrame, currentFrame;
    lastFrame = currentFrame = windowManager.get_time();
    // std::cout << currentFrame << std::endl;
    float dt = 0.0f;

    while (!quit)
    {
        windowManager.ProcessEvents();

        playerControlSystem.Update(dt);

        cameraControlSystem.update(dt);

        physicsSystem.Update(dt);

        renderSystem.Update(dt);

        windowManager.Update();

        currentFrame = windowManager.get_time();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    windowManager.Shutdown();

    return 0;
}
