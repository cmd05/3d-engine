#include <ecs/systems/PhysicsSystem.hpp>

#include <ecs/core/Coordinator.hpp>

#include <ecs/components/Gravity.hpp>
#include <ecs/components/RigidBody.hpp>
#include <ecs/components/Thrust.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/core/Event.hpp>

extern Coordinator gCoordinator;


void PhysicsSystem::Init()
{
}

void PhysicsSystem::Update(float dt)
{
    for (auto const& entity : m_entities)
    {
        auto& rigidBody = gCoordinator.get_component<RigidBody>(entity);
        auto& transform = gCoordinator.get_component<Transform>(entity);

        // Forces
        auto const& gravity = gCoordinator.get_component<Gravity>(entity);

        transform.position += rigidBody.velocity * dt;

        rigidBody.velocity += gravity.force * dt;
    }
}
