#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Vk
{
public:
	void createInstance();
	void setupDebugMessenger();
	void vkCleanUp();
	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

private:
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void compareExtensions(int reqExtensionCount, const char** reqExtensions, std::vector<VkExtensionProperties> extensions);
	std::vector<const char*> getRequiredExtensions();

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
};

