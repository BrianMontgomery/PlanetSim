#include "PSIMPCH.h"
#include "Core.h"

#include "Application.h"

#include "Logging/Log.h"

#include <set>
#include <cstdint> // Necessary for UINT32_MAX

//vulkan entry point
//--------------------------------------------------------------------------------------------------------------------------------
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


//Main Logic Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void Application::initWindow()
{
	PSIM_CORE_INFO("Initializing GLFW and Window");
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Planet Sim", nullptr, nullptr);
	PSIM_ASSERT(window, "Window failed to create");
}

void Application::initVulkan() 
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void Application::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	PSIM_CORE_WARN("Shutdown Initiated");
}

void Application::cleanUp() 
{
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	PSIM_CORE_INFO("Swapchain deleted");

	vkDestroyDevice(device, nullptr);
	PSIM_CORE_INFO("Device deleted");

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		PSIM_CORE_INFO("Debug Messenger deleted");
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	PSIM_CORE_INFO("Surface deleted");

	vkDestroyInstance(instance, nullptr);
	PSIM_CORE_INFO("Vulkan instance deleted");

	glfwDestroyWindow(window);

	glfwTerminate();
	PSIM_CORE_INFO("Glfw terminated");

	Log::shutdown();
}


//Instance Creation
//--------------------------------------------------------------------------------------------------------------------------------
void Application::createInstance() 
{
	PSIM_CORE_INFO("Initializing Vulkan instance and extensions");
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

	//getting extensions/layers
	std::vector<const char*> extensions = getInstanceExtensions();
	std::vector<const char*> vLayers = {};
	if (enableValidationLayers) {
		vLayers = getInstanceLayers();
	}

	//setting extensions/ debugging instance creation/destruction
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(vLayers.size());
		createInfo.ppEnabledLayerNames = vLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	//creating the instance
	PSIM_ASSERT((vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS), "Failed to create Vulkan instance!");
	PSIM_CORE_INFO("Vulkan Instance Created");
}

std::vector<const char*> Application::getInstanceExtensions()
{ 
	//required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> reqExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//add extra reqs here
	if (enableValidationLayers) {
		reqExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	//all supported extensions
	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

	std::vector<VkExtensionProperties> availExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availExtensions.data());

	//check for all required extensions
	bool allFound = 1;
	for (auto i = 0; i < reqExtensions.size(); i++) {
		bool found = false;

		for (const auto& extension : availExtensions) {
			if (strcmp(reqExtensions[i], extension.extensionName)) {
				PSIM_CORE_INFO("Added extension: {0}", reqExtensions[i]);
				found = true;
				break;
			}
		}

		//error message
		PSIM_ASSERT(allFound, "Extension: {0} not found!", reqExtensions[i]);
	}

	//success message
	PSIM_CORE_INFO("All required extensions found");

	return reqExtensions;
}

std::vector<const char*> Application::getInstanceLayers() 
{
	//available layers
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//checking if layers are available
	bool allFound = 1;
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				PSIM_CORE_INFO("Added layer: {0}", layerName);
				layerFound = true;
				break;
			}
		}

		//error message
		PSIM_ASSERT(layerFound, "Required Layer: {0} not available!", layerName);
	}

	//success message
	PSIM_CORE_INFO("All required layers available");

	return validationLayers;
}


//Debugging funcs
//--------------------------------------------------------------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
				PSIM_CORE_ERROR("validation layer: {0}", pCallbackData->pMessage);
				return VK_FALSE;
			}
			PSIM_CORE_WARN("validation layer: {0}", pCallbackData->pMessage);
			return VK_FALSE;
		}
		PSIM_CORE_INFO("validation layer: {0}", pCallbackData->pMessage);
		return VK_FALSE;
	}

	return VK_FALSE;
}

void Application::setupDebugMessenger() 
{
	if (!enableValidationLayers) { return; }

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	PSIM_ASSERT((CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) == VK_SUCCESS), "Failed to set up debug messenger!")
}

void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		PSIM_CORE_INFO("Created debug messenger");
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		PSIM_CORE_ERROR("Debug messenger could not be created!");
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}


//Surface Creation
//--------------------------------------------------------------------------------------------------------------------------------
void Application::createSurface()
{
	PSIM_ASSERT(glfwCreateWindowSurface(instance, window, nullptr, &surface) == VK_SUCCESS, "Failed to create window surface!");
	PSIM_CORE_INFO("Created Window Surface");
}


//Physical Device Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void Application::pickPhysicalDevice()
{
	PSIM_CORE_INFO("Picking GPU");
	//get devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	PSIM_ASSERT(deviceCount != 0, "Failed to find GPU's with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	//actually pick from available
	std::unordered_map<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			int score = rateDevice(device);
			candidates.insert(std::make_pair(score, device));
		}
	}

	// Check if the best candidate is suitable at all
	PSIM_ASSERT(candidates.begin()->first > 0, "Failed to find a suitable GPU!");
	physicalDevice = candidates.begin()->second;
	PSIM_CORE_INFO("GPU selected");
}

bool Application::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

int Application::rateDevice(VkPhysicalDevice device)
{
	//get features and properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}

bool Application::checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}


//Queue Funcs
//--------------------------------------------------------------------------------------------------------------------------------
Application::QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device)
{
	//create the queue family indeces
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	//check for graphics families in the queue
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		//if complete we are done
		if (indices.isComplete()) {
			break;
		}

		//if not go to the next
		i++;
	}

	//if none found, will not return with a value
	return indices;
}


//Logical Device Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void Application::createLogicalDevice() 
{
	//get families
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	//create queue info
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//create device info
	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; 
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures; 
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	//create device
	PSIM_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) == VK_SUCCESS, "Failed to create logical device!")
	PSIM_CORE_INFO("Created Logical Device");

	//get handles to the queues
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

//Swapchain Funcs
//--------------------------------------------------------------------------------------------------------------------------------
Application::SwapChainSupportDetails Application::querySwapChainSupport(VkPhysicalDevice device) 
{
	SwapChainSupportDetails details;

	//capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//formats

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
	//check for srgb
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
	//check for mailbox mode
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
	//wanted extent
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}  
	//fallback extent
	else {
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void Application::createSwapChain() 
{
//create/populate the swapChainSupport struct
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//define an image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//create/populate a vkSwapChainInfo struct
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	//create the swapchain
	PSIM_ASSERT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) == VK_SUCCESS, "Failed to create swap chain!");

	//retrieving images
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	//storing vars for later
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}