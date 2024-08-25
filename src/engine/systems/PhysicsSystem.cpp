#include <engine/systems/PhysicsSystem.hpp>

#include <engine/core/Scene.hpp>
#include <engine/core/SceneView.hpp>

#include <engine/components/Gravity.hpp>
#include <engine/components/RigidBody.hpp>
#include <engine/components/Transform.hpp>

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
