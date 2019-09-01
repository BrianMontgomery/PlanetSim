#pragma once
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

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

	void createInstance();
	std::vector<const char*> getInstanceExtensions();

	GLFWwindow* window;
	VkInstance instance;
};

