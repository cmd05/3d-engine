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

    // bool buttonStateChanged = false;

    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        gCoordinator.send_event(Events::Window::QUIT);
        // buttonStateChanged = true;
    }
    
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::W));
        // buttonStateChanged = true;
    }
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::W));
    // }

    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::A));
        // buttonStateChanged = true;
    } 
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::A));
    // }

    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::S));
        // buttonStateChanged = true;
    } 
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::S));
    // }
    
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::D));
        // buttonStateChanged = true;
    } 
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::D));
    // }
    
    if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::Q));
        // buttonStateChanged = true;
    } 
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::Q));
    // }

    if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::E));
        // buttonStateChanged = true;
    } 
    // else {
    //     mButtons.reset(static_cast<std::size_t>(InputButtons::E));
    // }

    // else
    // {
    //     mButtons.reset();
    //     // buttonStateChanged = false;
    // }

    // if(!buttonStateChanged)
    //     mButtons.reset();

    // if (buttonStateChanged)
    // {
        Event event(Events::Window::INPUT);
        event.set_param(Events::Window::Input::INPUT, mButtons); // set window input parameter to buttons pressed
        gCoordinator.send_event(event);
    // }
}
