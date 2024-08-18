#pragma once

#include <ecs/core/System.hpp>

class PhysicsSystem : public System
{
public:
    PhysicsSystem(Coordinator& coordinator): System(coordinator) {}

    void init();
    void update(float dt);
};
