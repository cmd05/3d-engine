#include <lib/utilities/DebugAssert.hpp>

#include <engine/window/WindowManager.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/gui/GUIMain.hpp>
#include <engine/config/Events.hpp>

// assuming single GLFW window
// static global variable - only visible to WindowManager.cpp
static bool g_window_focused = false;

bool WindowManager::is_window_focused() {
    return g_window_focused;
}

WindowManager::WindowManager(Scene &scene, InputHandler &input_handler): 
    m_scene{&scene}, m_input_handler{&input_handler} {}

void WindowManager::init(std::string const &window_title, unsigned int window_width,
                         unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y) {
    // initialize glfw library
    glfwInit();

    // set window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE); // maximize window when created

    m_window = glfwCreateWindow(window_width, window_height, window_title.c_str(), NULL, NULL);

    if (!m_window) {
        glfwTerminate();
        ASSERT_MESSAGE("Could not create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetCursorPosCallback(m_window, WindowManager::mouse_callback);
    glfwSetKeyCallback(m_window, WindowManager::key_callback);
    
    glfwSetFramebufferSizeCallback(m_window, WindowManager::framebuffer_size_callback);
    glfwSetWindowCloseCallback(m_window, WindowManager::close_callback);

    // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // tell GLFW to capture our mouse
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Create OpenGL Context
    glfwMakeContextCurrent(m_window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ASSERT_MESSAGE("Failed to load glad");

    // glfwSwapInterval(1); // use vsync and set interval

    // send event for OpenGL being initialized
    Event event {Events::Window::GL_INIT};
    m_scene->send_event(event);
}

void WindowManager::bind_gui(GUIMain &gui_main) {
    m_gui_main = &gui_main;
}

void WindowManager::update() {
    glfwSwapBuffers(m_window);

    // window_manager.process_events() is executed before window_manager.update(), 
    // so we get updated key actions for the same frame
    if(m_input_handler->get_key(GLFW_KEY_ESCAPE))
        close_window();

    if(m_input_handler->get_key(GLFW_KEY_LEFT_ALT) && is_window_focused()) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        g_window_focused = false;
    }

    if(m_input_handler->get_key(GLFW_KEY_ENTER) && !is_window_focused()) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        m_input_handler->get_mouse_data().first_mouse = true; // avoid large offsets on refocus
        g_window_focused = true;
    }
}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    Event event {Events::Window::RESIZED};
    event.set_param(Events::Window::Resized::WIDTH, width);
    event.set_param(Events::Window::Resized::HEIGHT, height);
    
    p_window_manager->m_scene->send_event(event);
}

void WindowManager::close_callback(GLFWwindow* window) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    p_window_manager->close_window();
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(p_window_manager->m_gui_main->io_want_capture_keyboard())
        return;

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    // update state of input handler
    p_window_manager->m_input_handler->handle_key_callback(key, scancode, action, mods);
}

void WindowManager::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    if(!is_window_focused())
        return;

    p_window_manager->m_input_handler->handle_scroll_callback(x_offset, y_offset);
}

void WindowManager::mouse_callback(GLFWwindow* window, double xpos_in, double ypos_in) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    if(!is_window_focused() || p_window_manager->m_gui_main->io_want_capture_mouse())
        return;

    p_window_manager->m_input_handler->handle_mouse_callback(xpos_in, ypos_in);
}

void WindowManager::shutdown() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowManager::close_window() {
    m_scene->send_event(Events::Window::QUIT);
    glfwSetWindowShouldClose(m_window, true);
}

void WindowManager::process_events() {
    glfwPollEvents();
    // update_keys();
}