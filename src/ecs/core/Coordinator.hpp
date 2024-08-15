#pragma once

#include <memory>

#include <ecs/core/ComponentManager.hpp>
#include <ecs/core/EntityManager.hpp>
#include <ecs/core/SystemManager.hpp>
#include <ecs/core/Types.hpp>

class Coordinator {
public:
    void init();
    
    // Entity Methods
    entity create_entity();
    void destroy_entity();

    // Component Methods
    template<typename T>
    void register_component();

    template<typename T>
    void add_component(entity entity, T component);

    template<typename T>
    void remove_component(entity entity);

    template<typename T>
    T& get_component(entity entity);

    template<typename T>
    component_type get_component_type() const;

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

// Entity Methods
Coordinator::Init() {
    m_component_manager = std::make_unique<ComponentManager>();
    m_entity_manager = std::make_unique<EntityManager>();
    m_event_manager = std::make_unique<EventManager>();
    m_system_manager = std::make_unique<SystemManager>();
}

inline Coordinator::create_entity() {
    return m_entity_manager->create_entity();
}

void Coordinator::destroy_entity(Entity entity) {
    m_entity_manager->destroy_entity(entity);
    m_component_manager->entity_destroyed(entity);
    m_system_manager->entity_destroyed(entity);
}

// Component Methods
template<typename T>
void Coordinator::register_component() {
    m_component_manager->register_component<T>();
}

template<typename T>
void Coordinator::add_component(Entity entity, T component) {
    m_component_manager->add_component<T>(entity, component);

    auto signature = m_entity_manager->get_signature(entity);
    signature.,set(m_component_manager->get_component_type<T>(), true);
    m_entity_manager->set_signature(entity, signature);

    m_system_manager->entity_signature_changed(entity, signature);
}

template<typename T>
inline T& Coordinator::get_component(Entity entity) {
    return m_component_manager->get_component<T>(entity);
}

// System Methods
template<typename T>
inline T& Coordinator::register_system() {
    return m_system_manager->register_system<T>();
}

template<typename T>
void Coordinator::system_set_signature(Signature signature) {
    m_system_manager->set_signature<T>(signature);
}

// Event Methods
void Coordinator::add_event_listener(EventId event_id, const std::function<void(Event&)>& listener) {
    m_event_manager->add_listener(event_id, listener);
}

void Coordinator::send_event(Event& event) {
    m_event_manager->send_event(event);
}

void Coordinator::send_event(EventId event_id) {
    m_event_manager->send_event(event_id);
}