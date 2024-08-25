#include <cassert>

#include <engine/ecs/core/EntityManager.hpp>

#include <engine/ecs/core/Types.hpp>

EntityManager::EntityManager() {
    m_sparse_array.fill(NO_SIGNATURE_MARKER);
}

Entity EntityManager::create_entity() {
    Entity entity;

    if(last_entity < MAX_ENTITIES)
        entity = last_entity++;
    else if(destroyed_entities.size()) {
        entity = destroyed_entities.front();
        destroyed_entities.pop();
    } else
        assert("Too many entities");

    // add zero initialized Signature for new entity
    set_signature(entity, {});

    return entity;
}

void EntityManager::destroy_entity(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range");

    entity_count_size_type removed_index = m_sparse_array[entity];

    assert(removed_index != NO_SIGNATURE_MARKER && "Entity does not exist");

    entity_count_size_type last_index = m_dense_entities.size() - 1;
    entity_count_size_type last_entity = m_dense_entities[last_index];

    // update sparse array
    m_sparse_array[last_entity] = removed_index;
    m_sparse_array[entity] = NO_SIGNATURE_MARKER;

    // copy last elements to removed index
    m_dense_entities[removed_index] = m_dense_entities[last_index];
    m_dense_signatures[removed_index] = m_dense_signatures[last_index];

    // remove last element
    m_dense_signatures.pop_back();
    m_dense_entities.pop_back();
    
    // update destroyed_entities
    destroyed_entities.push(entity);
}

void EntityManager::set_signature(Entity entity, Signature signature) {
    assert(entity < MAX_ENTITIES && "Entity out of range");
    
    entity_count_size_type index = m_sparse_array[entity];

    if(index == NO_SIGNATURE_MARKER) {
        m_sparse_array[entity] = m_dense_entities.size();
        m_dense_entities.push_back(entity);
        m_dense_signatures.push_back(signature);
    } else {
        m_dense_signatures[index] = signature;
    }
}

Signature EntityManager::get_signature(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entities out of range");

    return m_dense_signatures[m_sparse_array[entity]];
}

void EntityManager::clear() {
    for(Entity entity : m_dense_entities)
        m_sparse_array[entity] = NO_SIGNATURE_MARKER;
    
    m_dense_entities.clear();
    m_dense_signatures.clear();
}