#pragma once

struct InputConfig {
    // GLFW Keys information
    static constexpr int WIN_INPUT_KEYS_START = 0;
    static constexpr int WIN_INPUT_KEYS_END = 1024; // not included
    static constexpr int WIN_INPUT_KEYS_LEN = WIN_INPUT_KEYS_END;

    enum class BasicMovement {
        Forward,
        Right,
        Left,
        Backward,
        Up,
        Down
    };
};