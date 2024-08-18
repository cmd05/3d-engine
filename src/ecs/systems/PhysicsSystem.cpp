#include <ecs/systems/PhysicsSystem.hpp>

#include <ecs/core/Coordinator.hpp>

#include <ecs/components/Gravity.hpp>
#include <ecs/components/RigidBody.hpp>
#include <ecs/components/Transform.hpp>

void PhysicsSystem::Init()
{
}

void PhysicsSystem::Update(float dt)
{
    for (const auto& entity : m_entities)
    {
        auto& rigidBody = gCoordinator.get_component<RigidBody>(entity);
        auto& transform = gCoordinator.get_component<Transform>(entity);

        // Forces
        const auto& gravity = gCoordinator.get_component<Gravity>(entity);

        if(transform.position.y <= -100) {
            rigidBody.velocity.y *= -1;
        }

        transform.position += rigidBody.velocity * dt;

        rigidBody.velocity += gravity.force * dt;
    }
}
