#include <engine/ecs/core/Types.hpp>
#include <cstdint>

// Source: https://gist.github.com/Lee-R/3839813
// static function (non extern)
static constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count) {
    return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

// _hash creates uint32_t sized unique hashes for strings at compile time
// hash comparisons are faster than string comparison for the event system
static constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count) {
    return fnv1a_32(s, count);
}

namespace Events::Window {
    const EventId QUIT = "Events::Window::QUIT"_hash;
    const EventId RESIZED = "Events::Window::RESIZED"_hash;
    const EventId FOCUS_CHANGE = "Events::Window::FOCUS_CHANGE"_hash;
    const EventId GL_INIT = "Events::Window::GL_INIT"_hash;
}

namespace Events::Window::FocusChange {
    const ParamId FOCUSED = "Events::Window::FocusChange::FOCUSED"_hash;
}

namespace Events::Window::Resized {
    const ParamId WIDTH = "Events::Window::Resized::WIDTH"_hash;
    const ParamId HEIGHT = "Events::Window::Resized::HEIGHT"_hash;
}

/// Input

namespace Events::Input {
    const EventId KEYBOARD = "Events::Input::KEYBOARD"_hash;
    const EventId MOUSE = "Events::Input::MOUSE"_hash;
    const EventId SCROLL = "Events::Input::SCROLL"_hash;
};

namespace Events::Input::Keyboard {
    const ParamId KEYS = "Events::Input::Keyboard::KEYS"_hash;
}

namespace Events::Input::Mouse {
    const ParamId MOUSE_DATA = "Events::Input::Mouse::MOUSE_DATA"_hash;
}

namespace Events::Input::Scroll {
    const ParamId SCROLL_DATA = "Events::Input::Scroll::SCROLL_DATA"_hash;
}