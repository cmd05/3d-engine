
#include <engine/window/WindowManager.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>

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

    // // tell GLFW to capture our mouse
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create OpenGL Context
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void WindowManager::update() {
    glfwSwapBuffers(m_window);
}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        assert("WindowManager handler not set");

    Event event {Events::Window::RESIZED};
    event.set_param(Events::Window::Resized::WIDTH, width);
    event.set_param(Events::Window::Resized::HEIGHT, height);
    
    p_window_manager->ref_scene.send_event(event);
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        assert("WindowManager handler not set");
        
    if(key >= WIN_INPUT_KEYS_START && key < WIN_INPUT_KEYS_END) {
        if(action == GLFW_PRESS)
            p_window_manager->ref_input_handler.set_key(key);
        else if(action == GLFW_RELEASE) {
            p_window_manager->ref_input_handler.reset_key(key);
            p_window_manager->ref_input_handler.reset_key_processed(key);
        }
    }
    
    // TODO: fix logic in entire engine for window quit logic and process
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        p_window_manager->ref_scene.send_event(Events::Window::QUIT);
        glfwSetWindowShouldClose(window, true);
    }
}

// void WindowManager::update_keys() {
//     for(int key = WIN_INPUT_KEYS_START; key < WIN_INPUT_KEYS_END; key++) {
//         int action = glfwGetKey(m_window, key);

//         if(action == GLFW_PRESS) {
//             ref_input_handler.set_key(key);
//         } else if(action == GLFW_RELEASE) {
//             ref_input_handler.reset_key(key);
//             ref_input_handler.reset_key_processed(key);
//         }

//         // window quit logic
//         if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//             ref_scene.send_event(Events::Window::QUIT);
//             glfwSetWindowShouldClose(m_window, true);
//         }
//     }
// }

void WindowManager::scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        assert("WindowManager handler not set");

    p_window_manager->m_scroll_data.x_offset = x_offset;
    p_window_manager->m_scroll_data.y_offset = y_offset;

    Event event(Events::Window::Input::SCROLL);
    event.set_param(Events::Window::Input::Scroll::SCROLL_DATA, p_window_manager->m_scroll_data);
    p_window_manager->ref_scene.send_event(event);
}

void WindowManager::mouse_callback(GLFWwindow* window, double xpos_in, double ypos_in) {
    WindowManager* p_window_manager = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

    if(!p_window_manager)
        assert("WindowManager handler not set");

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