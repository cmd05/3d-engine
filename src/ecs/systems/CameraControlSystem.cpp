#include <ecs/systems/CameraControlSystem.hpp>

#include <ecs/components/Transform.hpp>
#include <ecs/core/Coordinator.hpp>
#include <ecs/core/Event.hpp>

extern Coordinator gCoordinator;

void CameraControlSystem::Init()
{
	gCoordinator.add_event_listener(METHOD_LISTENER(Events::Window::INPUT, CameraControlSystem::InputListener));
}

void CameraControlSystem::Update(float dt)
{
	for (auto& entity : m_entities)
	{
		auto& transform = gCoordinator.get_component<Transform>(entity);

		float speed = 20.0f;


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
	mButtons = event.get_param<std::bitset<8>>(Events::Window::Input::INPUT);
}