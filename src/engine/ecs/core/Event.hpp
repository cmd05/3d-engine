#pragma once

#include <any>
#include <unordered_map>

#include <engine/ecs/core/Types.hpp>

class Event {
public:
    Event() = delete;
    explicit Event(EventId type);

    template<typename T>
    void set_param(ParamId id, T value);
    
    template<typename T>
    T get_param(ParamId id);

    EventId get_type() const;

private:
    EventId m_type;
    std::unordered_map<ParamId, std::any> m_data;
};

template<typename T>
void Event::set_param(ParamId id, T value) {
    m_data[id] = value;
}

template<typename T>
T Event::get_param(ParamId id) {
    // get specific parameter from event
    // cast it to type specified by caller (using std::any_cast<T>) before returning
    return std::any_cast<T>(m_data[id]);
}