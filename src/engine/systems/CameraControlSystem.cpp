#include <engine/systems/CameraControlSystem.hpp>

#include <engine/core/Scene.hpp>
#include <engine/core/Event.hpp>
#include <engine/core/SceneView.hpp>

#include <engine/components/Transform.hpp>
#include <engine/components/Camera.hpp>

void CameraControlSystem::init()
{
    // macro expands to 
    // ref_scene.add_event_listener(Events::Window::INPUT, std::bind(&CameraControlSystem::input_listener, this, std::placeholders::_1));
    
    // std::placeholders::_1 is bound to the returned function object by std::bind.
    // https://en.cppreference.com/w/cpp/utility/functional/placeholders:
    // When used as an argument in a std::bind expression, the placeholder objects are stored in the generated function object,
    // and when that function object is invoked with unbound arguments,
    // each placeholder _N is replaced by the corresponding Nth unbound argument
    // Ex: In case of `CameraControlSystem::input_listener(Event& event)`,
    // std::placeholders::_1 will bind to Event& which will be sent to the function object of std::bind as the first argument
    
    ref_scene.add_event_listener(METHOD_LISTENER(Events::Camera::MOVEMENT, CameraControlSystem::input_listener));
}

void CameraControlSystem::update(float dt)
{
    for (Entity entity : SceneView<Camera, Transform>(ref_scene))
    {
        auto& transform = ref_scene.get_component<Transform>(entity);
        float speed = 120.0f;

        if (to_move.test(static_cast<std::size_t>(BasicMovement::Forward)))
            transform.position.z -= (dt * speed);
        else if (to_move.test(static_cast<std::size_t>(BasicMovement::Backward)))
            transform.position.z += (dt * speed);


        if (to_move.test(static_cast<std::size_t>(BasicMovement::Up)))
            transform.position.y += (dt * speed);
        else if (to_move.test(static_cast<std::size_t>(BasicMovement::Down)))
            transform.position.y -= (dt * speed);


        if (to_move.test(static_cast<std::size_t>(BasicMovement::Left)))
            transform.position.x -= (dt * speed);
        else if (to_move.test(static_cast<std::size_t>(BasicMovement::Right)))
            transform.position.x += (dt * speed);
    }

    to_move.reset(); // all movements for current frame have been done
}

void CameraControlSystem::input_listener(Event& event)
{
    // get window buttons input parameter
    // In WindowManager::ProcessEvents(), event has been set to:
        // Event event(Events::Camera::MOVEMENT);
        // event.set_param(Events::Camera::Movement::MOVES, moves);
        // ref_scene.send_event(event);
    // where moves is a std::bitset<8>. get_param() handles the any_cast in `Event` class 

    // update the state of the CameraControlSystem
    // the updation of camera position is done when CameraControlSystem::update() is called
    to_move = event.get_param<BasicMoves>(Events::Camera::Movement::MOVES);
}