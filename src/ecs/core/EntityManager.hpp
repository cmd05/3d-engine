#pragma once

#include <queue>
#include <array>
#include <vector>

#include <ecs/core/Types.hpp>

class EntityManager {
public:
    EntityManager();

    Entity create_entity();
    void destroy_entity(Entity entity);

    void set_signature(Entity entity, Signature signature);
    Signature get_signature(Entity entity);

    void clear();

private:
    std::queue<Entity> destroyed_entities{};

    // sparse set: map from entities to signatures
    std::array<entity_count_size_type, MAX_ENTITIES> m_sparse_array;
    std::vector<Entity> m_dense_entities;
    std::vector<Signature> m_dense_signatures;

    Entity last_entity = 0;
};