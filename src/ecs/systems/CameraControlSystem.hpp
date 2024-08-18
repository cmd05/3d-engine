#pragma once

#include <ecs/core/Coordinator.hpp>
#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

class CameraControlSystem : public System
{
public:
    CameraControlSystem(Coordinator& coordinator): System(coordinator) {}
    void init();
    void update(float dt);

private:
    BasicMoves to_move;
    void input_listener(Event& event);
};
