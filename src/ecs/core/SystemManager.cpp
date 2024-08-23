#include <ecs/core/SystemManager.hpp>

#include <ecs/core/Types.hpp>

void SystemManager::entity_destroyed(Entity entity) {
    // erase a destroyed entity from all system lists
    // system->m_entities is a set, so no check need
    // for(const auto& pair : m_systems) {
    //     const auto& system = pair.second;
    //     system->m_entities.erase(entity);
    // }
}

void SystemManager::entity_signature_changed(Entity entity, Signature entity_new_signature) {
    // notify each system that an entity's signature changed
    // for(const auto& pair : m_systems) {
    //     const auto& type = pair.first;
    //     const auto& system = pair.second;
    //     const auto& system_signature = m_signatures[type];

    //     // entity signature matches system signature - insert into set
    //     if((entity_new_signature & system_signature) == system_signature) {
    //         system->m_entities.insert(entity);
    //     } else {
    //         // entity signature does not match system signature - erase from set
    //         system->m_entities.erase(entity);
    //     }
    // }
}