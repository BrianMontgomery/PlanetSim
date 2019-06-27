#include "PSIMPCH.h"
#include "Core.h"

#include "Application.h"

#include "Logging/Log.h"

void Application::run()
{
	Log::init();

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
	createInstance();
}

void Application::createInstance()
{
	PSIM_CORE_INFO("Initializing Vulkan instance and extensions!\n");
	//creating info for vk instances
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Planet Sim";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//getting required glfw extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::cout << "Glfw required extensions:" << std::endl;
	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		std::cout << "\t" << glfwExtensions[i] << std::endl;
	} 
	std::cout << std::endl;

	//getting all supported extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available extensions:" << std::endl;
	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	} 
	std::cout << std::endl;

	//ensure that all extensions are there
	compareExtensions(glfwExtensionCount, glfwExtensions, extensions);

	//set the extensions in info
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	//create instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		PSIM_ERROR("Failed to create vk instance!");
	}
}

void Application::compareExtensions(int reqExtensionCount, const char** reqExtensions, std::vector<VkExtensionProperties> extensions)
{
	
	//check for all required extensions
	for(auto i = 0; i < reqExtensionCount; i++) {
		bool found = false;

		for (const auto& extension : extensions) {
			if (strcmp(reqExtensions[i], extension.extensionName)) {
				found = true;
			}
		}

		//error message
		if (!found) {
			PSIM_ERROR("Certain required GLFW extensions not found\n");
		}
	} 

	//success message
	PSIM_INFO("All required GLFW extensions found!\n");
	
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