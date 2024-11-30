#pragma once

#include <bitset>
#include <cstdint>
#include <limits>

// ECS

using entity_count_size_type = std::uint32_t;
// using entity_count_signed_size_type = std::int64_t;
using Entity = entity_count_size_type;
const Entity MAX_ENTITIES = 100;
const Entity MAX_ENTITIES_AFTER_CAMERA = MAX_ENTITIES - 1; // after user creates a `Camera` entity is created for RenderSystem

const entity_count_size_type NO_SIGNATURE_MARKER = MAX_ENTITIES;
const entity_count_size_type NO_COMPONENT_MARKER = MAX_ENTITIES; // entities: 0 to MAX_ENTITIES-1
                                                                 // valid as long as MAX_ENTITIES is not UINT32_MAX

using component_count_size_type = std::uint16_t; // MAX_COMPONENTS will fit within 16 bits
using ComponentType = component_count_size_type; // MAX_COMPONENTS will fit within 16 bits
const ComponentType MAX_COMPONENTS = 32;

using system_count_size_type = std::uint16_t; // max number of systems will fit within 16 bits
const system_count_size_type MAX_SYSTEMS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

// Events
using EventId = std::uint32_t;
using ParamId = std::uint32_t;

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)