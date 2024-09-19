#pragma once

#include <bitset>
#include <cstdint>
#include <limits>

// ECS

using entity_count_size_type = std::uint32_t;
// using entity_count_signed_size_type = std::int64_t;
using Entity = entity_count_size_type;
const Entity MAX_ENTITIES = 100;
const Entity MAX_ENTITIES_AFTER_CAMERA = MAX_ENTITIES - 1; // camera entity created in engine/ecs/systems/RenderSystem.cpp

const entity_count_size_type NO_SIGNATURE_MARKER = MAX_ENTITIES;
const entity_count_size_type NO_COMPONENT_MARKER = MAX_ENTITIES; // entities: 0 to MAX_ENTITIES-1
                                                                 // valid as long as MAX_ENTITIES is not UINT32_MAX

using component_count_size_type = std::uint16_t; // MAX_COMPONENTS will fit within 16 bits
using ComponentType = component_count_size_type; // MAX_COMPONENTS will fit within 16 bits
const ComponentType MAX_COMPONENTS = 32;

using system_count_size_type = std::uint16_t; // max number of systems will fit within 16 bits
const system_count_size_type MAX_SYSTEMS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

// Input
enum class InputButtons {
    W,
    A,
    S,
    D,
    Q,
    E
};

enum class BasicMovement {
    Forward,
    Right,
    Left,
    Backward,
    Up,
    Down
};

using BasicMoves = std::bitset<8>;

// Events
using EventId = std::uint32_t;
using ParamId = std::uint32_t;

// see CameraControlSystem::Init() for detailed explanation of METHOD_LISTENER
#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

// Source: https://gist.github.com/Lee-R/3839813
// static function (non extern)
static constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count) {
    return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

static constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count) {
    return fnv1a_32(s, count);
}

// _hash creates uint32_t sized unique hashes for strings at compile time
// hash comparisons are faster than string comparison for the event system
namespace Events::Window {
    const EventId QUIT = "Events::Window::QUIT"_hash;
    const EventId RESIZED = "Events::Window::RESIZED"_hash;
    const EventId INPUT = "Events::Window::INPUT"_hash;
}

namespace Events::Window::Input {
    const ParamId INPUT = "Events::Window::Input::INPUT"_hash;
}

namespace Events::Window::Resized {
    const ParamId WIDTH = "Events::Window::Resized::WIDTH"_hash;
    const ParamId HEIGHT = "Events::Window::Resized::HEIGHT"_hash;
}

namespace Events::Camera {
    const EventId MOVEMENT = "Events::Camera::MOVEMENT"_hash;
}

namespace Events::Camera::Movement {
    const ParamId MOVES = "Events::Camera::Movement::MOVES"_hash;
}