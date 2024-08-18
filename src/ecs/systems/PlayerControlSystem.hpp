#pragma once

#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

class PlayerControlSystem : public System
{
public:
    PlayerControlSystem(Coordinator& coordinator): System(coordinator) {}
    void Init();

    void Update(float dt);

private:
    std::bitset<8> mButtons;

    void InputListener(Event& event);
};
