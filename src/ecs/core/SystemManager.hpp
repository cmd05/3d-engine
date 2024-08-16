#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include <ecs/core/System.hpp>
#include <ecs/core/Types.hpp>

class SystemManager {
public:
    // System Modifiers
    template<typename T>
    T& register_system();

    template<typename T>
    void set_signature(Signature signature);
    
    // System Entities Modifiers
    void entity_destroyed(Entity entity);
    void entity_signature_changed(Entity entity, Signature entity_signature);

private:
    system_count_size_type m_system_count = 0;

    // map from system type to signature
    std::unordered_map<std::type_index, Signature> m_signatures{};

    // map from system type to system pointer
    std::unordered_map<std::type_index, std::unique_ptr<System>> m_systems{};
};

template<typename T>
T& SystemManager::register_system() {
    assert(m_system_count < MAX_SYSTEMS && "Registering systems exceeds MAX_SYSTEMS");

    std::type_index type = typeid(T);

    assert(m_systems.find(type) == m_systems.end() && "Registering system more than once");

    // store pointer to the system in unordered_map and return reference to the system
    auto it = m_systems.emplace(type, std::make_unique<T>()).first;

    m_system_count++;

    return *static_cast<T*>(it->second.get());
}

template<typename T>
void SystemManager::set_signature(Signature signature) {
    std::type_index type = typeid(T);

    assert(m_systems.find(type) != m_systems.end() && "Ssytem used before registered");

    // set the signature for this system
    m_signatures.insert({type, signature});
}