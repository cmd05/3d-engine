#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

#include <engine/input/InputHandler.hpp>

constexpr int DEFAULT_SCR_WIDTH = 1920;
constexpr int DEFAULT_SCR_HEIGHT = 1080;

class Scene;

// Handle main window and GLFW window contexts
class WindowManager
{
public:
    WindowManager(Scene& scene, InputHandler& input_handler): 
        ref_scene{scene}, m_input_handler{&input_handler} {}

    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);
    
    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }

    // GLFW callbacks
    static void mouse_callback(GLFWwindow* window, double xpos_in, double ypos_in);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    // void update_keys();

    GLFWwindow* get_window() { return m_window; }
    GLFWwindow* get_current_context() { return glfwGetCurrentContext(); }
    void set_current_context(GLFWwindow* window) { glfwMakeContextCurrent(window); }

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

private:
    GLFWwindow* m_window;
    InputHandler* m_input_handler;

    Scene& ref_scene;

    MouseData m_mouse_data{};
    ScrollData m_scroll_data{};
};