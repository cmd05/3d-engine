#pragma once

#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

class WindowManager
{
public:
    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);

    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }
private:
    GLFWwindow* m_window;

    std::bitset<8> m_buttons;
};
