#include <ecs/systems/CameraControlSystem.hpp>

#include <ecs/components/Transform.hpp>
#include <ecs/core/Coordinator.hpp>
#include <ecs/core/Event.hpp>

extern Coordinator gCoordinator;

void CameraControlSystem::Init()
{
    // macro expands to 
    // gCoordinator.add_event_listener(Events::Window::INPUT, std::bind(&CameraControlSystem::InputListener, this, std::placeholders::_1));
    
    // std::placeholders::_1 is bound to the returned function object by std::bind.
    // https://en.cppreference.com/w/cpp/utility/functional/placeholders:
    // When used as an argument in a std::bind expression, the placeholder objects are stored in the generated function object,
    // and when that function object is invoked with unbound arguments,
    // each placeholder _N is replaced by the corresponding Nth unbound argument
    // Ex: In case of `CameraControlSystem::InputListener(Event& event)`,
    // std::placeholders::_1 will bind to Event& which will be sent to the function object of std::bind as the first argument
    
    gCoordinator.add_event_listener(METHOD_LISTENER(Events::Window::INPUT, CameraControlSystem::InputListener));
}

void CameraControlSystem::Update(float dt)
{
    for (auto& entity : m_entities)
    {
        auto& transform = gCoordinator.get_component<Transform>(entity);

        float speed = 60.0f;


        if (mButtons.test(static_cast<std::size_t>(InputButtons::W)))
        {
            transform.position.z -= (dt * speed);
        }

        else if (mButtons.test(static_cast<std::size_t>(InputButtons::S)))
        {
            transform.position.z += (dt * speed);
        }


        if (mButtons.test(static_cast<std::size_t>(InputButtons::Q)))
        {
            transform.position.y += (dt * speed);
        }

        else if (mButtons.test(static_cast<std::size_t>(InputButtons::E)))
        {
            transform.position.y -= (dt * speed);
        }


        if (mButtons.test(static_cast<std::size_t>(InputButtons::A)))
        {
            transform.position.x -= (dt * speed);
        }

        else if (mButtons.test(static_cast<std::size_t>(InputButtons::D)))
        {
            transform.position.x += (dt * speed);
        }
    }
}

void CameraControlSystem::InputListener(Event& event)
{
    // get window buttons input parameter
    // In WindowManager::ProcessEvents(), event has been set to:
        // Event event(Events::Window::INPUT);
        // event.set_param(Events::Window::Input::INPUT, mButtons);
        // gCoordinator.send_event(event);
    // where mButtons is a std::bitset<8>. get_param() handles the any_cast in `Event` class 

    // update the state of the CameraControlSystem
    // the updation of camera position is done when CameraControlSystem::Update() is called
    mButtons = event.get_param<std::bitset<8>>(Events::Window::Input::INPUT);
}