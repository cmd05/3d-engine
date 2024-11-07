#include <GLFW/glfw3.h>

#include <engine/ecs/core/Event.hpp>
#include <engine/input/InputHandler.hpp>

void InputHandler::handle_key_callback(int key, int scancode, int action, int mods) {
    if(key >= WIN_INPUT_KEYS_START && key < WIN_INPUT_KEYS_END) {
        if(action == GLFW_PRESS)
            set_key(key);
        else if(action == GLFW_RELEASE) {
            reset_key(key);
            reset_key_processed(key);
        }
    }
}

void InputHandler::handle_mouse_callback(double xpos_in, double ypos_in) {
    if(m_mouse_data.first_mouse) {
        m_mouse_data.mouse_last_x = xpos_in;
        m_mouse_data.mouse_last_y = ypos_in;
        m_mouse_data.first_mouse = false;
    }

    double xoffset = xpos_in - m_mouse_data.mouse_last_x;
    double yoffset = m_mouse_data.mouse_last_y - ypos_in; // reversed since y-coordinates go from bottom to top

    // set data
    m_mouse_data.x_offset = xoffset;
    m_mouse_data.y_offset = yoffset;

    m_mouse_data.mouse_last_x = xpos_in;
    m_mouse_data.mouse_last_y = ypos_in;

    // send event
    Event event(Events::Input::MOUSE);
    event.set_param(Events::Input::Mouse::MOUSE_DATA, m_mouse_data);
    m_scene->send_event(event);
}

void InputHandler::handle_scroll_callback(double x_offset, double y_offset) {
    m_scroll_data.x_offset = x_offset;
    m_scroll_data.y_offset = y_offset;

    Event event(Events::Input::SCROLL);
    event.set_param(Events::Input::Scroll::SCROLL_DATA, m_scroll_data);
    m_scene->send_event(event);
}