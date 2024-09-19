#pragma once

#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

#include <engine/ecs/systems/InputHandler.hpp>


class Scene;

class WindowManager
{
public:
    WindowManager(Scene& scene, InputHandler& input_handler): 
        ref_scene{scene}, ref_input_handler{input_handler} {}

    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);
    
    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
private:
    GLFWwindow* m_window;

    InputHandler& ref_input_handler;
    Scene& ref_scene;

    std::bitset<8> m_buttons;
};
