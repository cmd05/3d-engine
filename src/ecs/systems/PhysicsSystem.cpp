#include <ecs/systems/PhysicsSystem.hpp>

#include <ecs/core/Coordinator.hpp>
#include <ecs/core/SceneView.hpp>

#include <ecs/components/Gravity.hpp>
#include <ecs/components/RigidBody.hpp>
#include <ecs/components/Transform.hpp>

void PhysicsSystem::init() {}

void PhysicsSystem::update(float dt)
{
    for (const auto& entity : SceneView<RigidBody, Transform, Gravity>(ref_gcoordinator))
    {
        auto& rigid_body = ref_gcoordinator.get_component<RigidBody>(entity);
        auto& transform = ref_gcoordinator.get_component<Transform>(entity);

        // forces
        const auto& gravity = ref_gcoordinator.get_component<Gravity>(entity);

        // bounce of "ground"
        if(transform.position.y <= -100) {
            rigid_body.velocity.y *= -1;
        }

        // update quantities
        transform.position += rigid_body.velocity * dt;
        rigid_body.velocity += gravity.force * dt;
    }
}
