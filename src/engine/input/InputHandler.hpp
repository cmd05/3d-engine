#pragma once

// TODO: InputManager will be the link / "glue" between Window input (key states array) and scene (updation systems)
// to perform some actions while sanely managing the key states

// TODO: think about moving InputHandler to appropriate folder in engine/ instead of engine/ecs/systems
// maybe engine/input

// #include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Types.hpp>

constexpr int WIN_KEYS_LEN = 1024;

class InputHandler {
public:
    // InputHandler(Scene& scene): ref_scene{scene} {

    // }

    // TODO: Add interface for these methods to Scene so we can directly do scene.input_set_key() [set_key()]
    void set_key(std::size_t key) { input_keys[key] = true; }
    void reset_key(std::size_t key) { input_keys[key] = false; }

    void set_key_processed(std::size_t key) { input_keys_processed[key] = true; }
    void reset_key_processed(std::size_t key) { input_keys_processed[key] = false; }

    bool get_key(std::size_t key) const { return input_keys[key]; }
    bool get_key_processed(std::size_t key) const { return input_keys_processed[key]; }

    // TODO: implement support for execution for key by lambdas
    // react_key_noprocess
    // react_key_processed
private:
    // default initialize all array elements to zero (false)
    bool input_keys[WIN_INPUT_KEYS_LEN] = {0};
    bool input_keys_processed[WIN_INPUT_KEYS_LEN] = {0};
    
    // TODO: right now InputHandler does not use `Scene`
    // However, we will have to initialize it with a scene reference, to use the scene object 
    // Scene& ref_scene;
};