#pragma once

#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

class PlayerControlSystem : public System
{
public:
    PlayerControlSystem(Scene& scene): System(scene) {}
    void init();

    void update(float dt);

private:
    std::bitset<8> m_buttons;

    void input_listener(Event& event);
};
