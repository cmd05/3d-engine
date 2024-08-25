#include <engine/core/Event.hpp>

#include <engine/core/Types.hpp>

Event::Event(EventId type): m_type{type} {}

EventId Event::get_type() const {
    return m_type;
}