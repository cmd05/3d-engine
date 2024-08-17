#include <ecs/systems/PlayerControlSystem.hpp>

#include <ecs/components/Player.hpp>
#include <ecs/components/Thrust.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/core/Coordinator.hpp>
#include <ecs/core/Event.hpp>

extern Coordinator gCoordinator;


void PlayerControlSystem::Init()
{
    gCoordinator.add_event_listener(METHOD_LISTENER(Events::Window::INPUT, PlayerControlSystem::InputListener));
}

void PlayerControlSystem::Update(float dt)
{
    //for (auto& entity : mEntities)
    //{
    //	auto& transform = gCoordinator.Get<Transform>(entity);


    //	if (mButtons.test(static_cast<std::size_t>(InputButtons::W)))
    //	{
    //		transform.position.z += (dt * 10.0f);
    //	}

    //	else if (mButtons.test(static_cast<std::size_t>(InputButtons::S)))
    //	{
    //		transform.position.z -= (dt * 10.0f);
    //	}


    //	if (mButtons.test(static_cast<std::size_t>(InputButtons::Q)))
    //	{
    //		transform.position.y += (dt * 10.0f);
    //	}

    //	else if (mButtons.test(static_cast<std::size_t>(InputButtons::E)))
    //	{
    //		transform.position.y -= (dt * 10.0f);
    //	}


    //	if (mButtons.test(static_cast<std::size_t>(InputButtons::A)))
    //	{
    //		transform.position.x += (dt * 10.0f);
    //	}

    //	else if (mButtons.test(static_cast<std::size_t>(InputButtons::D)))
    //	{
    //		transform.position.x -= (dt * 10.0f);
    //	}
    //}
}

void PlayerControlSystem::InputListener(Event& event)
{
    mButtons = event.get_param<std::bitset<8>>(Events::Window::Input::INPUT);
}
