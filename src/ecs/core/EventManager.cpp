#include <ecs/core/EventManager.hpp>

#include <ecs/core/Event.hpp>
#include <ecs/core/Types.hpp>

void EventManager::add_listener(EventId event_id, const std::function<void(Event&)>& listener) {
    m_listeners[event_id].push_back(listener);
}

void EventManager::send_event(Event& event) {
    EventId type = event.get_type();

    // call all listeners for which listen to EventId == `type`
    for(const auto& listener : m_listeners[type])
        listener(event); // call the listeners with the `Event` as its argument
}

void EventManager::send_event(EventId event_id) {
    Event event {event_id};

    for(const auto& listener : m_listeners[event_id])
        listener(event);
}