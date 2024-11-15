#include <engine/ecs/systems/PhysicsSystem.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/SceneView.hpp>

#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Transform.hpp>

void PhysicsSystem::init() {}

void PhysicsSystem::update(float dt)
{
    for (const auto& entity : SceneView<Components::RigidBody, Components::Transform, Components::Gravity>(*m_scene))
    {
        auto& rigid_body = m_scene->get_component<Components::RigidBody>(entity);
        auto& transform = m_scene->get_component<Components::Transform>(entity);

        // forces
        const auto& gravity = m_scene->get_component<Components::Gravity>(entity);

        // bounce of "ground"
        if(transform.position.y <= -100) {
            rigid_body.velocity.y *= -1;
        }

        // update quantities
        transform.position += rigid_body.velocity * dt;
        rigid_body.velocity += gravity.force * dt;
    }
}
