#pragma once

#include <cassert>
#include <queue>
#include <stdexcept>
// #include <iostream>
#include <map>

#include <ecs/core/Types.hpp>

class EntityManager {
public:
    EntityManager();

    Entity create_entity();
    void destroy_entity(Entity entity);
    void set_signature(Entity entity, Signature signature);
    Signature get_signature(Entity entity);

private:
    std::queue<Entity> destroyed_entities{};

    std::map<Entity, Signature> m_signatures{};
    Entity last_entity = 0;
};