#pragma once

#include <ecs/core/Types.hpp>

class Coordinator;

// A system is any functionality that iterates upon a list of entities
// with a certain signature of components.
class System
{
public:
    System(Coordinator& coordinator) : ref_gcoordinator(coordinator) {}
protected:
    Coordinator& ref_gcoordinator;
};