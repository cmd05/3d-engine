#include <cassert>

#include <ecs/core/EntityManager.hpp>

#include <ecs/core/Types.hpp>

EntityManager::EntityManager() {}

Entity EntityManager::create_entity() {
    Entity id;

    if(last_entity < MAX_ENTITIES)
        id = last_entity++;
    else if(destroyed_entities.size()) {
        id = destroyed_entities.front();
        destroyed_entities.pop();
    } else
        assert("Too many entities");

    return id;
}

void EntityManager::destroy_entity(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range");

    m_signatures[entity].reset();
    destroyed_entities.push(entity);
}

void EntityManager::set_signature(Entity entity, Signature signature) {
    assert(entity < MAX_ENTITIES && "Entity out of range");

    m_signatures[entity] = signature;
}

// get_signature may be called with non existent key (entity) for m_signatures
// in that case a new key must be inserted
Signature EntityManager::get_signature(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entities out of range");

    // std::map: Inserts value_type(key, T()) if the key does not exist
    return m_signatures[entity];
}