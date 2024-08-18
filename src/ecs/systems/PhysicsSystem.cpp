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
        auto& rigidBody = ref_gcoordinator.get_component<RigidBody>(entity);
        auto& transform = ref_gcoordinator.get_component<Transform>(entity);

        // Forces
        const auto& gravity = ref_gcoordinator.get_component<Gravity>(entity);

        if(transform.position.y <= -100) {
            rigidBody.velocity.y *= -1;
        }

        transform.position += rigidBody.velocity * dt;

        rigidBody.velocity += gravity.force * dt;
    }
}
