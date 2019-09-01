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
	PSIM_CORE_INFO("Initializing GLFW and Window!");
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Planet Sim", nullptr, nullptr);
	PSIM_ASSERT(window, "Window failed to create!");
}

void Application::initVulkan() 
{
	createInstance();
}

void Application::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void Application::cleanUp() 
{
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();

	Log::shutdown();
}

void Application::createInstance() 
{
	PSIM_CORE_INFO("Initializing Vulkan instance and extensions!");
	//creating info for vk instances
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//getting extensions
	std::vector<const char*> extensions = getInstanceExtensions();

	//setting extensions
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.enabledLayerCount = 0;

	//creating the instance
	PSIM_CORE_INFO("Creating Vulkan Instance!");
	PSIM_ASSERT((vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS), "Failed to create Vulkan instance!");
}

std::vector<const char*> Application::getInstanceExtensions()
{ 
	//required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> reqExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//add extra reqs here


	//all supported extensions
	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

	std::vector<VkExtensionProperties> availExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availExtensions.data());

	//check for all required extensions
	for (auto i = 0; i < reqExtensions.size(); i++) {
		bool found = false;

		for (const auto& extension : availExtensions) {
			if (strcmp(reqExtensions[i], extension.extensionName)) {
				found = true;
			}
		}

		//error message
		if (!found) {
			PSIM_ERROR("Certain required GLFW extensions not found");
		}
	}

	//success message
	PSIM_CORE_INFO("All required extensions found!");

	return reqExtensions;
}