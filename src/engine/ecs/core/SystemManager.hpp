#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include <engine/ecs/core/System.hpp>
#include <engine/ecs/core/Types.hpp>

class Scene;

class SystemManager {
public:
    // System Modifiers
    template<typename T>
    T& register_system(Scene& scene);
    
    // void entity_destroyed(Entity entity);
    // void entity_signature_changed(Entity entity, Signature entity_signature);

private:
    system_count_size_type m_system_count = 0;
    std::unordered_map<std::type_index, std::unique_ptr<System>> m_systems{};
};

template<typename T>
T& SystemManager::register_system(Scene& scene) {
    assert(m_system_count < MAX_SYSTEMS && "Registering systems exceeds MAX_SYSTEMS");

    std::type_index type = typeid(T);

    assert(m_systems.find(type) == m_systems.end() && "Registering system more than once");

    // store pointer to the system in unordered_map and return reference to the system
    auto it = m_systems.emplace(type, std::make_unique<T>(scene)).first; // initialize system with scene

    m_system_count++;

    return *static_cast<T*>(it->second.get());
}