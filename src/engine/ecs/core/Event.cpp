#include <engine/ecs/core/Event.hpp>

#include <engine/ecs/core/Types.hpp>

Event::Event(EventId type): m_type{type} {}

EventId Event::get_type() const {
    return m_type;
}