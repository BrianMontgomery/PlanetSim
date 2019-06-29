#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Vk/Vk.h"

const int WIDTH = 800;
const int HEIGHT = 600;

class Application
{
public:
	void run();

private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();

	GLFWwindow* window;
	Vk vk;
};

