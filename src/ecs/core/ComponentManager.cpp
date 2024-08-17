#include <ecs/core/ComponentManager.hpp>

#include <ecs/core/ComponentArray.hpp>
#include <ecs/core/Types.hpp>

void ComponentManager::entity_destroyed(Entity entity) {
    // Notify *each component array* that an entity has been destroyed
    // If it has a component for that entity, it will remove it
    for (const auto& pair : m_component_arrays)
    {
        const auto& component = pair.second;

        component->entity_destroyed(entity);
    }
}