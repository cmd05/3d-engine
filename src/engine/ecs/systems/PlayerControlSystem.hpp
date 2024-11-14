#pragma once

#include <engine/ecs/core/System.hpp>
#include <engine/ecs/core/Event.hpp>

class PlayerControlSystem : public System {
public:
    PlayerControlSystem(Scene& scene): System(scene) {}
    void init();

    void update(float dt);

private:
    void input_listener(Event& event);
};
