#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

#include <engine/input/InputHandler.hpp>

class Scene;
class GUIMain;

// Handle main window and GLFW window contexts
class WindowManager
{
public:
    WindowManager(Scene& scene, InputHandler& input_handler);

    void init(std::string const& window_title, unsigned int window_width,
        unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y);
    void bind_gui(GUIMain& gui_main);

    void update();
    void process_events();
    void shutdown();
    float get_time() const { return glfwGetTime(); }

    void close_window();

    // GLFW callbacks
    static void mouse_callback(GLFWwindow* window, double xpos_in, double ypos_in);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void close_callback(GLFWwindow* window);

    GLFWwindow* get_window() { return m_window; }
    GLFWwindow* get_current_context() { return glfwGetCurrentContext(); }
    void set_current_context(GLFWwindow* window) { glfwMakeContextCurrent(window); }
    void get_framebuffer_size(int& width, int& height) { glfwGetFramebufferSize(m_window, &width, &height); }

    static bool is_window_focused();
private:
    InputHandler* const m_input_handler;
    Scene* const m_scene;

    GLFWwindow* m_window;
    GUIMain* m_gui_main;

    // bool m_window_focused = false;
};