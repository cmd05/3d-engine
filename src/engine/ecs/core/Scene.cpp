#include <memory>
#include <functional>

#include <engine/ecs/core/Scene.hpp>

#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/core/Event.hpp>

// Entity Methods
Scene::Scene() {
    m_component_manager = std::make_unique<ComponentManager>();
    m_entity_manager = std::make_unique<EntityManager>();
    m_event_manager = std::make_unique<EventManager>();
    m_system_manager = std::make_unique<SystemManager>();
}

Entity Scene::create_entity() {
    return m_entity_manager->create_entity();
}

void Scene::destroy_entity(Entity entity) {
    m_entity_manager->destroy_entity(entity);
    m_component_manager->entity_destroyed(entity);
}

// Event Methods
void Scene::add_event_listener(EventId event_id, const std::function<void(Event&)>& listener) {
    m_event_manager->add_listener(event_id, listener);
}

void Scene::send_event(Event& event) {
    m_event_manager->send_event(event);
}

void Scene::send_event(EventId event_id) {
    m_event_manager->send_event(event_id);
}

bool Scene::has_all_components(Entity entity) const {
    return m_component_manager->has_all_components(entity);
}

Signature Scene::get_entity_signature(Entity entity) const {
    return m_entity_manager->get_signature(entity);
}