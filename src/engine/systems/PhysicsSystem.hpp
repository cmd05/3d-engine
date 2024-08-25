#pragma once

#include <engine/core/System.hpp>

class PhysicsSystem : public System
{
public:
    PhysicsSystem(Scene& scene): System(scene) {}

    void init();
    void update(float dt);
};
