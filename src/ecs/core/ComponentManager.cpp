#include <any>
#include <memory>
#include <unordered_map>
// #include <iostream>
#include <typeindex>

#include <ecs/core/ComponentManager.hpp>

#include <ecs/core/ComponentArray.hpp>
#include <ecs/core/Types.hpp>

void ComponentManager::entity_destroyed(Entity entity) {
    // Notify *each component array* that an entity has been destroyed
    // If it has a component for that entity, it will remove it
    for (auto const& pair : m_component_arrays)
    {
        auto const& component = pair.second;

        component->entity_destroyed(entity);
    }
}