#pragma once
#define GLFW_INCLUDE_VULKAN
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
	void createInstance();
	void compareExtensions(int reqExtensionCount, const char** reqExtensions, std::vector<VkExtensionProperties> extensions);
	void mainLoop();
	void cleanUp();

	GLFWwindow* window;
	VkInstance instance;
};

