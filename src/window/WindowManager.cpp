#include <glad/glad.h>

#include <window/WindowManager.hpp>

#include <ecs/core/Coordinator.hpp>
#include <ecs/core/Event.hpp>

extern Coordinator gCoordinator; // extern gCoordinator can be accessed from main.cpp as it is not static


// TODO: Return error to caller
void WindowManager::Init(
    std::string const& windowTitle, unsigned int windowWidth, unsigned int windowHeight, unsigned int windowPositionX,
    unsigned int windowPositionY)
{
    glfwInit();

    mWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create OpenGL Context
    glfwMakeContextCurrent(mWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(2);

    // Configure OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}


void WindowManager::Update()
{
    glfwSwapBuffers(mWindow);
}


void WindowManager::Shutdown()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void WindowManager::ProcessEvents()
{
    glfwPollEvents();
    
    mButtons.reset(); // reset all bits of the bitset (do not retain previously pressed keys)
    
    BasicMoves moves{};

    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        gCoordinator.send_event(Events::Window::QUIT);
    
    // Camera Movement
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Forward));

    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Left));

    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Backward));
    
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Right));
    
    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Up));

    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
        moves.set(static_cast<std::size_t>(BasicMovement::Down));
    
    // always send movement (empty `moves` means no movement)
    Event event(Events::Camera::MOVEMENT);
    event.set_param(Events::Camera::Movement::MOVES, moves);
    gCoordinator.send_event(event);
}
