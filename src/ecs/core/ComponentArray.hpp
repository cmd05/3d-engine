#pragma once

#include <ecs/core/Types.hpp>

#include <lib/simple-vector/SimpleVector.hpp>

#include <cassert>
#include <unordered_map>

// ComponentArray is an packed array (i.e it has no holes)

// An interface class (IComponentArray) is needed so that ComponentManager
// can store a generic ComponentArray
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entity_destroyed(Entity entity) = 0; // must be implemented by ComponentArray
};

// A component array for component of type T
// Ex: ComponentArray<Transform>
template<typename T>
class ComponentArray : public IComponentArray {
public:
    void insert_data(Entity entity, T component);
    void remove_data(Entity entity);
    
    T& get_data(Entity entity);
    void entity_destroyed(Entity entity) override;

    void clear();
    
private:
    SimpleVector<T, entity_count_size_type, std::allocator<T>> m_component_vector{};

    std::unordered_map<Entity, entity_count_size_type> m_entity_to_index_map{};
    std::unordered_map<entity_count_size_type, Entity> m_index_to_entity_map{};
};

template<typename T>
void ComponentArray<T>::insert_data(Entity entity, T component) {
    assert(!(m_component_vector.capacity() >= MAX_ENTITIES && m_component_vector.size() == MAX_ENTITIES) && "component array size exceeded MAX_ENTITIES");

    assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() && "Component added to same entity more than once.");

    // put new entry at the end
    entity_count_size_type new_index = m_component_vector.size();
    m_entity_to_index_map[entity] = new_index;
    m_index_to_entity_map[new_index] = entity;

    m_component_vector.push_back(component);
}

template<typename T>
void ComponentArray<T>::remove_data(Entity entity) {
    assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Component does not exist for given entity");

    // copy element at the end into deleted element's place to maintain density
    entity_count_size_type index_removed_entity = m_entity_to_index_map[entity];
    entity_count_size_type index_last_elem = m_component_vector.size() - 1;

    m_component_vector[index_removed_entity] = m_component_vector[index_last_elem];

    // update map
    Entity entity_last_elem = m_index_to_entity_map[index_last_elem];
    m_entity_to_index_map[entity_last_elem] = index_removed_entity;
    m_index_to_entity_map[index_removed_entity] = entity_last_elem;

    m_entity_to_index_map.erase(entity);
    m_index_to_entity_map.erase(index_last_elem);

    m_component_vector.pop();
}

template<typename T>
T& ComponentArray<T>::get_data(Entity entity) {
    auto it = m_entity_to_index_map.begin();
    assert((it = m_entity_to_index_map.find(entity)) != m_entity_to_index_map.end() && "Retrieving non-existent component");
    
    return m_component_vector[it->second];
}

template<typename T>
void ComponentArray<T>::entity_destroyed(Entity entity) {
    if(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end())
        remove_data(entity);
}

template<typename T>
void ComponentArray<T>::clear() {
    m_component_vector.clear();
}