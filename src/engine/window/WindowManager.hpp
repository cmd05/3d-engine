#pragma once

#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

class Scene;

class WindowManager
{
public:
    // store scene as reference (for send_event())
    WindowManager(Scene& scene): ref_scene{scene} {}

    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);
    

    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }
private:
    GLFWwindow* m_window;
    Scene& ref_scene;

    std::bitset<8> m_buttons;
};
