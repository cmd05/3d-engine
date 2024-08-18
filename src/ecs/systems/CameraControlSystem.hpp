#pragma once

#include <ecs/core/Coordinator.hpp>
#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

class CameraControlSystem : public System
{
public:
    CameraControlSystem(Coordinator& coordinator): System(coordinator) {}

    void Init();

    void Update(float dt);

private:
    std::bitset<8> mButtons;

    void InputListener(Event& event);
};
