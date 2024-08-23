#include <ecs/systems/PlayerControlSystem.hpp>

#include <ecs/core/Coordinator.hpp>
#include <ecs/core/Event.hpp>

#include <ecs/core/SceneView.hpp>


void PlayerControlSystem::init()
{
    // ref_gcoordinator.add_event_listener(METHOD_LISTENER(Events::Window::INPUT, PlayerControlSystem::input_listener));
}

void PlayerControlSystem::update(float dt)
{
    //for (auto& entity : SceneView<Player, Transform>(ref_gcoordinator))
    //{
    //	auto& transform = ref_gcoordinator.Get<Transform>(entity);


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

void PlayerControlSystem::input_listener(Event& event)
{
    // m_buttons = event.get_param<std::bitset<8>>(Events::Window::Input::INPUT);
}
