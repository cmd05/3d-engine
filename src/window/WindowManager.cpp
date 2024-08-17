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

    bool buttonStateChanged = true;

    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE))
    {
        gCoordinator.send_event(Events::Window::QUIT);
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_W))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::W));
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_A))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::A));
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_S))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::S));
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_D))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::D));
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_Q))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::Q));
    }
    else if (glfwGetKey(mWindow, GLFW_KEY_E))
    {
        mButtons.set(static_cast<std::size_t>(InputButtons::E));
    }
    else
    {
        buttonStateChanged = false;
    }

    if (buttonStateChanged)
    {
        Event event(Events::Window::INPUT);
        event.set_param(Events::Window::Input::INPUT, mButtons);
        gCoordinator.send_event(event);
    }
}
