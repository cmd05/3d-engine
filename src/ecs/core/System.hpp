#pragma once

#include <set>

#include <ecs/core/Types.hpp>

class Coordinator;

// A system is any functionality that iterates upon a list of entities
// with a certain signature of components.
class System
{
public:
    // Removing entity from std::set is faster than a list
    // inserting in std::set for existing element does nothing.
    // If trying to erase when it doesn’t exist, it does nothing

    // All entities with a certain signature
    System(Coordinator& coordinator) : ref_gcoordinator(coordinator) {}
    
    std::set<Entity> m_entities;
protected:
    Coordinator& ref_gcoordinator;
};