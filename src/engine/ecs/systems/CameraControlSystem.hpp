#pragma once

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/System.hpp>
#include <engine/ecs/core/Event.hpp>

class CameraControlSystem : public System
{
public:
    CameraControlSystem(Scene& scene): System(scene) {}
    void init();
    void update(float dt);

private:
    BasicMoves to_move;
    void input_listener(Event& event);
};
