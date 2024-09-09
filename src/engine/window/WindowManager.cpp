#include <glad/glad.h>

#include <engine/window/WindowManager.hpp>

#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/Event.hpp>

// TODO: Return error to caller
void WindowManager::init(std::string const& windowTitle, unsigned int windowWidth,
    unsigned int windowHeight, unsigned int windowPositionX, unsigned int windowPositionY)
{
    // setup window
    glfwInit();
    m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);

    // set window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create OpenGL Context
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}


void WindowManager::update()
{
    glfwSwapBuffers(m_window);
}


void WindowManager::shutdown()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowManager::process_events()
{
    glfwPollEvents();
    
    m_buttons.reset(); // reset all bits of the bitset (do not retain previously pressed keys)
    
    BasicMoves moves{};

    // Window quit
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        ref_scene.send_event(Events::Window::QUIT);
    
    // FIX: use generic way to transmit pressed keys and send events for them
    // Camera Movement
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Forward));

    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Left));

    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Backward));
    
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Right));
    
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Up));

    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Down));
    
    // only send camera movement if their is movement input
    if(moves.any()) {
        Event event(Events::Camera::MOVEMENT);
        event.set_param(Events::Camera::Movement::MOVES, moves);
        ref_scene.send_event(event);
    }
}
