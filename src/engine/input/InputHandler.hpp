#pragma once

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Types.hpp>

class InputHandler {
public:
    struct MouseData {
        bool first_mouse = true;
        double mouse_last_x = 0;
        double mouse_last_y = 0;

        double x_offset;
        double y_offset;
    };

    struct ScrollData {
        double x_offset = 0;
        double y_offset = 0;
    };

    InputHandler(Scene& scene);

    void set_key(std::size_t key) { input_keys[key] = true; }
    void reset_key(std::size_t key) { input_keys[key] = false; }

    void set_key_processed(std::size_t key) { input_keys_processed[key] = true; }
    void reset_key_processed(std::size_t key) { input_keys_processed[key] = false; }

    bool get_key(std::size_t key) const { return input_keys[key]; }
    bool get_key_processed(std::size_t key) const { return input_keys_processed[key]; }

    void handle_key_callback(int key, int scancode, int action, int mods);
    void handle_mouse_callback(double xpos_in, double ypos_in);
    void handle_scroll_callback(double x_offset, double y_offset);

    MouseData& get_mouse_data() { return m_mouse_data; }

    // TBD: to listen for continuous presses: get_key
    // to listen for single press: get_key and reset_key after that (see breakout implementation)
    
    // TBD: implement support for execution for key by lambdas
    // react_key_noprocess
    // react_key_processed
private:
    Scene* const m_scene;

    // default initialize all array elements to zero (false)
    bool input_keys[WIN_INPUT_KEYS_LEN] = {0};
    bool input_keys_processed[WIN_INPUT_KEYS_LEN] = {0};

    // default initialize `m_mouse_data` and `m_scroll_data`
    MouseData m_mouse_data{};
    ScrollData m_scroll_data{};
};