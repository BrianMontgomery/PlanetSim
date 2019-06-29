#include "PSIMPCH.h"
#include "Core.h"

#include "Application.h"

#include "Logging/Log.h"

void Application::run()
{
	//initialize logging (the three day bug...)
	Log::init();

	//init funcs
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

void Application::initWindow()
{
	PSIM_CORE_INFO("Initializing GLFW and Window!\n");
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Planet Sim", nullptr, nullptr);
	PSIM_ASSERT(window, "Window failed to create!\n");
}

void Application::initVulkan()
{
	vk.createInstance();
	vk.setupDebugMessenger();
}

void Application::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void Application::cleanUp() 
{
	vk.vkCleanUp();

	glfwDestroyWindow(window);

	glfwTerminate();

	Log::shutdown();
}