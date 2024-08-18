#pragma once

#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

class Coordinator;

class WindowManager
{
public:
    // store coordinator as reference (for send_event())
    WindowManager(Coordinator& coordinator): ref_gcoordinator{coordinator} {}

    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);
    

    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }
private:
    GLFWwindow* m_window;
    Coordinator& ref_gcoordinator;

    std::bitset<8> m_buttons;
};
