#include <engine/window/WindowManager.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/gui/GUIMain.hpp>
#include <lib/utilities/DebugAssert.hpp>

// assuming single GLFW window
// static global variable - only visible to WindowManager.cpp
static bool g_window_focused = false;

bool WindowManager::is_window_focused() {
    return g_window_focused;
}

void WindowManager::init(std::string const& window_title, unsigned int window_width,
    unsigned int window_height, unsigned int window_position_x, unsigned int window_position_y) {
    // setup window
    glfwInit();
    m_window = glfwCreateWindow(window_width, window_height, window_title.c_str(), NULL, NULL);

    // set window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetCursorPosCallback(m_window, WindowManager::mouse_callback);
    glfwSetKeyCallback(m_window, WindowManager::key_callback);
    
    glfwSetFramebufferSizeCallback(m_window, WindowManager::framebuffer_size_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create OpenGL Context
    glfwMakeContextCurrent(m_window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ASSERT_MESSAGE("Failed to load glad");
    }
}

void WindowManager::bind_gui(GUIMain &gui_main) {
    m_gui_main = &gui_main;
}

void WindowManager::update() {
    glfwSwapBuffers(m_window);

    // since window_manager.process_events() is before window_manager.update()
    // we can handle only key setting in key_callback and respond here
    // TODO: fix logic in entire engine for window quit logic and process
    if(m_input_handler->get_key(GLFW_KEY_ESCAPE)) {
        ref_scene.send_event(Events::Window::QUIT);
        glfwSetWindowShouldClose(m_window, true);
    }

    // check that imgui is not focused/hovered
    if(m_input_handler->get_key(GLFW_KEY_LEFT_ALT) && g_window_focused) {
        // Event event {Events::Window::RESIZED};
        // event.set_param(Events::Window::Resized::WIDTH, width);
        // event.set_param(Events::Window::Resized::HEIGHT, height);
    
        // p_window_manager->ref_scene.send_event(event);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        g_window_focused = false;
    }

    if(m_input_handler->get_key(GLFW_KEY_ENTER) && !g_window_focused) {
        // Event event {Events::Window::RESIZED};
        // event.set_param(Events::Window::Resized::WIDTH, width);
        // event.set_param(Events::Window::Resized::HEIGHT, height);
    
        // p_window_manager->ref_scene.send_event(event);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    
    p_window_manager->ref_scene.send_event(event);
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    // const ImGuiIO& io = p_window_manager->m_gui_main->get_io();
    // io.AddKeyEvent(key, action);

    // // if(io.WantCaptureKeyboard)
    if(p_window_manager->m_gui_main->io_want_capture_keyboard())
        return;
    // ImGui_ImplGlfw_KeyCallback();

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    if(key >= WIN_INPUT_KEYS_START && key < WIN_INPUT_KEYS_END) {
        if(action == GLFW_PRESS)
            p_window_manager->m_input_handler->set_key(key);
        else if(action == GLFW_RELEASE) {
            p_window_manager->m_input_handler->reset_key(key);
            p_window_manager->m_input_handler->reset_key_processed(key);
        }
    }
}

void WindowManager::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    p_window_manager->m_scroll_data.x_offset = x_offset;
    p_window_manager->m_scroll_data.y_offset = y_offset;

    Event event(Events::Window::Input::SCROLL);
    event.set_param(Events::Window::Input::Scroll::SCROLL_DATA, p_window_manager->m_scroll_data);
    p_window_manager->ref_scene.send_event(event);
}

// TODO: window manager sends mouse callback data to InputHandler which transmits an `Event`
void WindowManager::mouse_callback(GLFWwindow* window, double xpos_in, double ypos_in) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        ASSERT_MESSAGE("WindowManager handler not set");

    if(p_window_manager->m_mouse_data.first_mouse) {
        p_window_manager->m_mouse_data.mouse_last_x = xpos_in;
        p_window_manager->m_mouse_data.mouse_last_y = ypos_in;
        p_window_manager->m_mouse_data.first_mouse = false;
    }

    double xoffset = xpos_in - p_window_manager->m_mouse_data.mouse_last_x;
    double yoffset = p_window_manager->m_mouse_data.mouse_last_y - ypos_in; // reversed since y-coordinates go from bottom to top

    // set data
    p_window_manager->m_mouse_data.x_offset = xoffset;
    p_window_manager->m_mouse_data.y_offset = yoffset;

    p_window_manager->m_mouse_data.mouse_last_x = xpos_in;
    p_window_manager->m_mouse_data.mouse_last_y = ypos_in;

    // send event
    Event event(Events::Window::Input::MOUSE);
    event.set_param(Events::Window::Input::Mouse::MOUSE_DATA, p_window_manager->m_mouse_data);
    p_window_manager->ref_scene.send_event(event);
}

void WindowManager::shutdown() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowManager::process_events() {
    glfwPollEvents();
    // update_keys();
}