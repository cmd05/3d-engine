#pragma once

#include <engine/core/Types.hpp>

class Scene;

// A system is any functionality that iterates upon a list of entities
// with a certain signature of components.
class System
{
public:
    System(Scene& scene) : ref_scene(scene) {}
protected:
    Scene& ref_scene;
};