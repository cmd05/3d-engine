#pragma once

#include <functional>
#include <list>
#include <unordered_map>

#include <ecs/core/Event.hpp>
#include <ecs/core/Types.hpp>

class EventManager {
public:
    void add_listener(EventId event_id, const std::function<void(Event&)>& listener);
    void send_event(Event& event);
    void send_event(EventId event_id);

private:
    std::unordered_map<EventId, std::list<std::function<void(Event&)>>> m_listeners;
};