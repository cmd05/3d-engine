#pragma once

#include <engine/ecs/core/Types.hpp>

class Scene;

// A system is any functionality that iterates upon a list of entities
// with a certain signature of components.
class System
{
public:
    System(Scene& scene) : ref_scene(scene) {}
protected:
    // TODO: use scene pointer `Scene* m_scene` and modify for all systems
    Scene& ref_scene;
};