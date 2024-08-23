#pragma once

#include <utility>
#include <cassert>
#include <unordered_map>
#include <array>
// #include <vector>

#include <lib/simple-vector/SimpleVector.hpp>

#include <ecs/core/Types.hpp>

// change when using `SimpleVector`
using vector_entity_iterator = std::vector<Entity>::iterator;
using vector_entity_const_iterator = std::vector<Entity>::const_iterator;

// An interface class (IComponentArray) is needed so that ComponentManager
// can store a generic ComponentArray
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entity_destroyed(Entity entity) = 0; // pure virtual functions

    virtual vector_entity_iterator begin() = 0;
    virtual vector_entity_iterator end() = 0;

    virtual entity_count_size_type size() const = 0;

    virtual bool has_component(Entity entity) const = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    ComponentArray();

    void insert_data(Entity entity, T component);
    void remove_data(Entity entity);
    
    void entity_destroyed(Entity entity);
    
    void clear();
    
    entity_count_size_type size() const;

    // provide begin and end iterators
    vector_entity_iterator begin() { return m_dense_entities.begin(); }
    vector_entity_iterator end() { return m_dense_entities.end(); }
    
    vector_entity_const_iterator cbegin() const { return m_dense_entities.cbegin(); }
    vector_entity_const_iterator cend() const { return m_dense_entities.cend(); }
private:
    std::array<Entity, MAX_ENTITIES> m_sparse_array;
    SimpleVector<std::pair<Entity, T>, entity_count_size_type> m_component_vector;
    // std::vector<std::pair<Entity, T>> m_component_vector;
};

template<typename T>
ComponentArray<T>::ComponentArray() {
    m_sparse_array.fill(NO_COMPONENT_MARKER);
}

template<typename T>
entity_count_size_type ComponentArray<T>::size() const {
    return m_component_vector.size();
}

template<typename T>
void ComponentArray<T>::insert_data(Entity entity, T component) {
    assert(!(m_component_vector.capacity() >= MAX_ENTITIES && m_component_vector.size() == MAX_ENTITIES) && "component array size exceeded MAX_ENTITIES");

    assert(m_sparse_array[entity] == NO_COMPONENT_MARKER && "Component added to same entity more than once.");
    
    // put new entry
    entity_count_size_type new_index = m_component_vector.size();
    m_sparse_array[entity] = new_index;
    m_component_vector.push_back(std::make_pair(entity, component));
}

template<typename T>
void ComponentArray<T>::remove_data(Entity entity) {
    assert(m_sparse_array[entity] != NO_COMPONENT_MARKER && "Component does not exist for given entity");

    // copy element at the end into deleted element's place to maintain density
    entity_count_size_type index_removed_entity = m_sparse_array[entity];
    entity_count_size_type index_last_elem = m_component_vector.size() - 1;

    // update sparse set
    Entity entity_last_elem = m_component_vector[index_last_elem].first;
    m_sparse_array[entity_last_elem] = index_removed_entity;
    m_component_vector[index_removed_entity] = m_component_vector[index_last_elem];

    m_sparse_array[entity] = NO_COMPONENT_MARKER;
    m_component_vector.pop_back();
}

template<typename T>
T& ComponentArray<T>::get_data(Entity entity) {
    entity_count_size_type index = m_sparse_array[entity];

    assert(index != NO_COMPONENT_MARKER && "Retrieving non existent component");
    
    T& component = m_component_vector[index].second;

    return component;
}

template<typename T>
void ComponentArray<T>::entity_destroyed(Entity entity) {
    if(m_sparse_array[entity] != NO_COMPONENT_MARKER)
        remove_data(entity);
}

template<typename T>
void ComponentArray<T>::clear() {
    m_sparse_array.fill(NO_COMPONENT_MARKER);
    m_component_vector.clear();
}