#include <engine/ecs/systems/PlayerControlSystem.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/ecs/core/SceneView.hpp>


void PlayerControlSystem::init()
{
    // ref_scene.add_event_listener(METHOD_LISTENER(Events::Input::, PlayerControlSystem::input_listener));
}

void PlayerControlSystem::update(float dt)
{
    //for (auto& entity : SceneView<Player, Transform>(ref_scene))
    //{
    //	auto& transform = ref_scene.Get<Transform>(entity);


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
}
