#pragma once

#include <engine/core/Scene.hpp>
#include <engine/core/System.hpp>
#include <engine/core/Event.hpp>

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
