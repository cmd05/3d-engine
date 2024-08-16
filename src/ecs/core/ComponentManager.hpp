#pragma once

#include <any>
#include <memory>
#include <unordered_map>
// #include <iostream>
#include <typeindex>

#include <ecs/core/ComponentArray.hpp>
#include <ecs/core/Types.hpp>

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

    void entity_destroyed(Entity entity);

private:
    template<typename T>
    ComponentArray<T>* get_component_array();

private:
    std::unordered_map<std::type_index, ComponentType> m_component_types{};
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_component_arrays{};

    ComponentType m_next_component_type{};
};


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

    assert(m_component_types.find(type) != m_component_types.end() && "Component not registered before use");
    
    // return this component's type - used for creating signatures
    return m_component_types[type];
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