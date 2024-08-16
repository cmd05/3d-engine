#pragma once

#include <any>
#include <unordered_map>

#include <ecs/core/Types.hpp>

class Event {
public:
    Event() = delete;
    explicit Event(EventId type);

    template<typename T>
    void set_param(EventId id, T value);
    
    template<typename T>
    T get_param(EventId id);

    EventId get_type() const;

private:
    EventId m_type{};
    std::unordered_map<EventId, std::any> m_data{};
};

template<typename T>
void Event::set_param(EventId id, T value) {
    m_data[id] = value;
}

template<typename T>
T Event::get_param(EventId id) {
    return std::any_cast<T>(m_data[id]);
}