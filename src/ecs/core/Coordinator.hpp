#pragma once

#include <memory>
#include <functional>
#include <utility>

#include <ecs/core/ComponentManager.hpp>
#include <ecs/core/EntityManager.hpp>
#include <ecs/core/SystemManager.hpp>
#include <ecs/core/EventManager.hpp>

#include <ecs/core/Types.hpp>
#include <ecs/core/Event.hpp>

class Coordinator {
public:
    void init();

public:
    // Entity Methods
    Entity create_entity();
    Signature get_entity_signature(Entity entity) const;
    void destroy_entity(Entity entity);

public:
    // Component Methods
    template<typename ...Args>
    void register_component();

    template<typename T>
    void add_component(Entity entity, T component);

    template<typename T>
    void remove_component(Entity entity);

    template<typename T>
    T& get_component(Entity entity);

    template<typename T>
    bool has_component(Entity entity) const;

    bool has_all_components(Entity entity) const;
    
    template<typename ...ComponentTypes>
    Signature get_components_signature() const;

    template<typename T>
    ComponentType get_component_type() const;

    template<typename ...ComponentTypes>
    std::pair<vector_entity_iterator, vector_entity_iterator> get_smallest_component_array();

public:
    // System Methods
    template<typename T>
    T& register_system();

public:
    // Event Methods
    void add_event_listener(EventId event_id, const std::function<void(Event&)>& listener); // pass listener by reference

    void send_event(Event& event);

    void send_event(EventId event_id);

private:
    std::unique_ptr<ComponentManager> m_component_manager;
    std::unique_ptr<EntityManager> m_entity_manager;
    std::unique_ptr<EventManager> m_event_manager;
    std::unique_ptr<SystemManager> m_system_manager;
};

template<typename ...ComponentTypes>
Signature Coordinator::get_components_signature() const {
    return m_component_manager->get_signature<ComponentTypes...>();
}

template<typename ...Args>
void Coordinator::register_component() {
    (m_component_manager->register_component<Args>(), ...);
}

template<typename T>
void Coordinator::add_component(Entity entity, T component) {
    m_component_manager->add_component<T>(entity, component);

    auto signature = m_entity_manager->get_signature(entity);
    signature.set(m_component_manager->get_component_type<T>(), true);
    m_entity_manager->set_signature(entity, signature);

    // m_system_manager->entity_signature_changed(entity, signature);
}

template<typename T>
void Coordinator::remove_component(Entity entity) {
    m_component_manager->remove_component<T>(entity);

    auto signature = m_entity_manager->get_signature(entity);
    signature.set(m_component_manager->get_component_type<T>(), false);
    m_entity_manager->set_signature(entity, signature);

    // m_system_manager->entity_signature_changed(entity, signature);
}

template<typename T>
T& Coordinator::get_component(Entity entity) {
    return m_component_manager->get_component<T>(entity);
}

template<typename T>
ComponentType Coordinator::get_component_type() const {
    return m_component_manager->get_component_type<T>();
}

template<typename T>
T& Coordinator::register_system() {
    return m_system_manager->register_system<T>(*this); // register system with the coordinator
}

template<typename ...ComponentTypes>
std::pair<vector_entity_iterator, vector_entity_iterator> Coordinator::get_smallest_component_array() {
    return m_component_manager->get_smallest_component_array<ComponentTypes...>();
}

template<typename T>
bool Coordinator::has_component(Entity entity) const {
    return m_component_manager->has_component<T>(entity);
}