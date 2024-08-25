#include <engine/ecs/systems/PhysicsSystem.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/SceneView.hpp>

#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Transform.hpp>

void PhysicsSystem::init() {}

void PhysicsSystem::update(float dt)
{
    for (const auto& entity : SceneView<RigidBody, Transform, Gravity>(ref_scene))
    {
        auto& rigid_body = ref_scene.get_component<RigidBody>(entity);
        auto& transform = ref_scene.get_component<Transform>(entity);

        // forces
        const auto& gravity = ref_scene.get_component<Gravity>(entity);

        // bounce of "ground"
        if(transform.position.y <= -100) {
            rigid_body.velocity.y *= -1;
        }

        // update quantities
        transform.position += rigid_body.velocity * dt;
        rigid_body.velocity += gravity.force * dt;
    }
}
