#include <engine/core/ComponentManager.hpp>

#include <engine/core/ComponentArray.hpp>
#include <engine/core/Types.hpp>

void ComponentManager::entity_destroyed(Entity entity) {
    // Notify each component array that an entity has been destroyed
    // If it has a component for that entity, it will remove it
    for (const auto& pair : m_component_arrays) {
        const auto& component = pair.second;

        component->entity_destroyed(entity);
    }
}

component_count_size_type ComponentManager::count_registered_components() const {
    return m_component_types.size();
}

bool ComponentManager::has_all_components(Entity entity) const {
    for(const auto& pair : m_component_arrays)
        if(!pair.second->has_component(entity))
            return false;

    return true;
}