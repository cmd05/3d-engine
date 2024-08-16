#pragma once

#include <glad/glad.h>
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

private:
	GLFWwindow* mWindow;

	std::bitset<8> mButtons;
};
