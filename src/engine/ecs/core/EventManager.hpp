#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

#include <engine/ecs/core/Event.hpp>
#include <engine/ecs/core/Types.hpp>

class EventManager {
public:
    void add_listener(EventId event_id, const std::function<void(Event&)>& listener);
    void send_event(Event& event);
    void send_event(EventId event_id);

private:
    std::unordered_map<EventId, std::vector<std::function<void(Event&)>>> m_listeners;
};