#pragma once

#include <GLFW/glfw3.h>
#include <bitset>
#include <string>

class WindowManager
{
public:
    void Init(
        std::string const& windowTitle, unsigned int windowWidth, unsigned int windowHeight,
        unsigned int windowPositionX, unsigned int windowPositionY);

    void Update();

    void ProcessEvents();

    void Shutdown();

    float get_time() const { return glfwGetTime(); }
private:
    GLFWwindow* mWindow;

    std::bitset<8> mButtons;
};
