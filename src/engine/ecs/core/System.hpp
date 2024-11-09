#pragma once

#include <engine/ecs/core/Types.hpp>

class Scene;

// A system is any functionality that iterates upon a list of entities
// with a certain signature of components.
class System
{
public:
    System(Scene& scene) : m_scene{&scene} {}
protected:
    Scene* const m_scene;
};