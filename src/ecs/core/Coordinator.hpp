#pragma once

#include <memory>
#include <functional>

#include <ecs/core/ComponentManager.hpp>
#include <ecs/core/EntityManager.hpp>
#include <ecs/core/SystemManager.hpp>
#include <ecs/core/EventManager.hpp>
#include <ecs/core/Types.hpp>
#include <ecs/core/Event.hpp>

class Coordinator {
public:
    void init();
    
    // Entity Methods
    Entity create_entity();
    void destroy_entity(Entity entity);

    // Component Methods
    template<typename T>
    void register_component();

    template<typename T>
    void add_component(Entity entity, T component);

    template<typename T>
    void remove_component(Entity entity);

    template<typename T>
    T& get_component(Entity entity);

    template<typename T>
    ComponentType get_component_type() const;

    // System Methods
    template<typename T>
    T& register_system();

    template<typename T>
    void set_system_signature(Signature signature);

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

// Component Methods
template<typename T>
void Coordinator::register_component() {
    m_component_manager->register_component<T>();
}

template<typename T>
void Coordinator::add_component(Entity entity, T component) {
    m_component_manager->add_component<T>(entity, component);

    auto signature = m_entity_manager->get_signature(entity);
    signature.set(m_component_manager->get_component_type<T>(), true);
    m_entity_manager->set_signature(entity, signature);

    m_system_manager->entity_signature_changed(entity, signature);
}

template<typename T>
void Coordinator::remove_component(Entity entity) {
    m_component_manager->remove_component<T>(entity);

    auto signature = m_entity_manager->get_signature(entity);
    signature.set(m_component_manager->get_component_type<T>(), false);
    m_entity_manager->set_signature(entity, signature);

    m_system_manager->entity_signature_changed(entity, signature);
}

template<typename T>
T& Coordinator::get_component(Entity entity) {
    return m_component_manager->get_component<T>(entity);
}

template<typename T>
ComponentType Coordinator::get_component_type() const {
    return m_component_manager->get_component_type<T>();
}

// System Methods
template<typename T>
T& Coordinator::register_system() {
    return m_system_manager->register_system<T>();
}

template<typename T>
void Coordinator::set_system_signature(Signature signature) {
    m_system_manager->set_signature<T>(signature);
}