#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>
#include <algorithm>
#include <utility>
#include <vector>

#include <engine/ecs/core/ComponentArray.hpp>
#include <engine/ecs/core/Types.hpp>

// #include <lib/utilities/DebugAssert.hpp>

class ComponentManager {
public:
    template<typename T>
    void register_component();

    template<typename T>
    void deregister_and_clear_component_array();


    template<typename T>
    ComponentType get_component_type();

    template<typename T>
    void add_component(Entity entity, T component);

    template<typename T>
    void remove_component(Entity entity);

    template<typename T>
    T& get_component(Entity entity);


    template<typename ...ComponentTypes>
    Signature get_signature();


    template<typename T>
    bool has_component(Entity entity);
    
    bool has_all_components(Entity entity) const;

    void entity_destroyed(Entity entity);

    
    component_count_size_type count_registered_components() const;
    
    template<typename T>
    entity_count_size_type size_component_array() const;


    template<typename ...ComponentTypes>
    std::pair<vector_entity_iterator, vector_entity_iterator> get_smallest_component_array();

private:
    template<typename T>
    ComponentArray<T>* get_component_array();

private:
    std::unordered_map<std::type_index, ComponentType> m_component_types;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_component_arrays;

    ComponentType m_next_component_type{};
};

template<typename ...ComponentTypes>
Signature ComponentManager::get_signature() {
    Signature signature;
    
    if(sizeof...(ComponentTypes) == 0)
        // set signature for all registered component types
        for(auto pair : m_component_types)
            signature.set(pair.second, true);
    else
        // get signature for requested component types
        (signature.set(get_component_type<ComponentTypes>(), true), ...);

    return signature;
}

template<typename ...ComponentTypes>
std::pair<vector_entity_iterator, vector_entity_iterator> ComponentManager::get_smallest_component_array() {
    if(sizeof...(ComponentTypes) == 0) { // if no component types specified, check all component types
        auto it = std::min_element( m_component_arrays.begin(), m_component_arrays.end(), 
            [](const auto& pair1, const auto& pair2) { return pair1.second->size() < pair2.second->size(); } );
        
        // ASSERT(it->second->size() > 0, "Empty component array: " << it->first.name());

        // In case of empty vector, begin() iterator is equal to end() 
        return std::make_pair(it->second->begin(), it->second->end());
    } else {
        std::vector<std::type_index> requested_types;
        (requested_types.push_back(typeid(ComponentTypes)), ...);

        entity_count_size_type min_components = MAX_ENTITIES;
        
        std::type_index min_type = requested_types[0];
        IComponentArray* comp_array_ptr;

        for(std::type_index type : requested_types) {
            entity_count_size_type comp_array_size = m_component_arrays[type]->size();

            if(comp_array_size < min_components) {
                min_components = comp_array_size;
                min_type = type;
                comp_array_ptr = m_component_arrays[type].get();
            }
        }

        // ASSERT(comp_array_ptr->size() > 0, "Empty component array: " << min_type.name());

        return std::make_pair(comp_array_ptr->begin(), comp_array_ptr->end());
    }
}

template<typename T>
entity_count_size_type ComponentManager::size_component_array() const {
    std::type_index type = typeid(T);

    auto it = m_component_arrays.begin();
    assert((it = m_component_arrays.find(type) != m_component_arrays.end()) && "Component not registered.");

    return it->second->size();   
}

// Private Methods
template<typename T>
ComponentArray<T>* ComponentManager::get_component_array() {
    std::type_index type = typeid(T);

    assert(m_component_types.find(type) != m_component_types.end() && "Component not registered before use.");

    return static_cast<ComponentArray<T>*>(m_component_arrays[type].get());
}

// Public Methods
template<typename T>
void ComponentManager::register_component() {
    std::type_index type = typeid(T);

    assert(m_component_types.find(type) == m_component_types.end() && "Registering component more than once");

    // add the component type to the component type map
    m_component_types.insert({type, m_next_component_type});
    
    m_component_arrays.emplace(type, std::make_unique<ComponentArray<T>>());

    // increment the value so that next component will be registered different
    ++m_next_component_type;
}

// deregister this component array and all its components
// the systems previously using these components, must update their signatures
template<typename T>
void ComponentManager::deregister_and_clear_component_array() {
    std::type_index type = typeid(T);

    auto it1 = m_component_arrays.find(type);

    if(it1 != m_component_arrays.end()) {
        ComponentArray<T>* component_array_ptr = static_cast<ComponentArray<T>*>(it1->second.get());
        component_array_ptr->Clear(); // clear component array data

        m_component_arrays.erase(it1);
    }
    
    auto it2 = m_component_types.find(type);
    if(it2 != m_component_types.end())
        m_component_types.erase(it2);
}

template<typename T>
ComponentType ComponentManager::get_component_type() {
    std::type_index type = typeid(T);

    auto it = m_component_types.begin();
    assert((it = m_component_types.find(type)) != m_component_types.end() && "Component not registered before use");
    
    // return this component's type - used for creating signatures
    return it->second;
}

template<typename T>
void ComponentManager::add_component(Entity entity, T component) {
    // add a component to the array for an entity
    get_component_array<T>()->insert_data(entity, component);
}

template<typename T>
void ComponentManager::remove_component(Entity entity) {
    // remove a component from the array for an entity
    get_component_array<T>()->remove_data(entity);
}

template<typename T>
T& ComponentManager::get_component(Entity entity) {
    return get_component_array<T>()->get_data(entity);
}

template<typename T>
bool ComponentManager::has_component(Entity entity) {
    return get_component_array<T>()->has_component(entity);
}