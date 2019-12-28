//in the premake but included here for redundancy

//core files
#include "PSIMPCH.h"
#include "Core.h"

#include "Logging/Log.h"
#include "VkRender.h"

//vendor files
#include <tinyObjLoader/tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>

//non-core std-lib
#include <chrono>
#include <fstream>
#include <set>
#include <cstdint> // Necessary for UINT32_MAX

//need full paths from solution for resources
const std::string MODEL_PATH = "resources/models/chalet.obj";
const char* TEXTURE_PATH = "resources/textures/chalet.jpg";

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const * pAllocator)
{
	return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}


//GLOBAL namespaces
namespace std {
	template<> struct hash<VkRender::Vertex> {
		size_t operator()(VkRender::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

//static funcs
//--------------------------------------------------------------------------------------------------------------------------------
static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	//get new size of window
	auto app = reinterpret_cast<VkRender*>(glfwGetWindowUserPointer(window));

	//tell app resize has happened
	app->framebufferResized = true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	//give VK debug info if severity: verbose, warning, error through PSIM_CORE
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

static std::vector<char> readFileByteCode(const std::string& filename)
{
	//open file
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	PSIM_ASSERT(file.is_open(), "Failed to open file!");

	//allocate buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//read the file
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	//close the file
	file.close();

	return buffer;
}

//--------------------------------------------------------------------------------------------------------------------------------


//VkRender Funcs
//--------------------------------------------------------------------------------------------------------------------------------
VkRender::VkRender()
{
	initVulkan();
}

VkRender::~VkRender()
{
	cleanUp();
}

//--------------------------------------------------------------------------------------------------------------------------------


//Main Logic Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::initVulkan()
{
	//all of the functions necessary to create an initial vulkan context
	initWindow();
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	//createSwapchainImageViews();
	//createRenderPass();
	//createDescriptorSetLayout();
	//createGraphicsPipeline();
	//createDepthResources();
	//createFramebuffers();
	//createCommandPool();
	//createTextureImage();
	//createTextureImageView();
	//createTextureSampler();
	//loadModel();
	//createVertexBuffer();
	//createIndexBuffer();
	//createUniformBuffers();
	//createDescriptorPool();
	//createDescriptorSets();
	//createCommandBuffers();
	//createSyncObjects();

	PSIM_CORE_INFO("Vulkan Initialization Complete!");
}

void VkRender::initWindow()
{
	PSIM_CORE_INFO("Initializing GLFW and Window");
	glfwInit();

	//window creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Planet Sim", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	PSIM_ASSERT(window, "Window failed to create");
}

bool VkRender::mainLoop()
{
	glfwPollEvents();
	//drawFrame();

	return (glfwWindowShouldClose(window));
}

void VkRender::cleanUp()
{
	device.waitIdle();
	
	//final cleanup of Vulkan resources
	cleanupSwapChain();

	//device.destroySampler(textureSampler, nullptr);
	//PSIM_CORE_INFO("Texture sampler destroyed");

	//device.destroyImageView(textureImageView, nullptr);
	//PSIM_CORE_INFO("Texture image view destroyed");

	//device.destroyImage(textureImage, nullptr);
	//device.freeMemory(textureImageMemory, nullptr);
	//PSIM_CORE_INFO("Image destroyed");

	//device.destroyDescriptorSetLayout(descriptorSetLayout, nullptr);

	//device.destroyBuffer(indexBuffer, nullptr);
	//device.freeMemory(indexBufferMemory, nullptr);
	//PSIM_CORE_INFO("Index Buffer Destroyed");

	//device.destroyBuffer(vertexBuffer, nullptr);
	//device.freeMemory(vertexBufferMemory, nullptr);
	//PSIM_CORE_INFO("Vertex Buffer Destroyed");

	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	//	device.destroySemaphore(renderFinishedSemaphores[i], nullptr);
	//	device.destroySemaphore(imageAvailableSemaphores[i], nullptr);
	//	device.destroyFence(inFlightFences[i], nullptr);
	//}
	PSIM_CORE_INFO("Sync Objects deleted");

	//device.destroyCommandPool(commandPool, nullptr);
	//PSIM_CORE_INFO("Command Pool deleted");
	
	device.destroy(nullptr);
	PSIM_CORE_INFO("Device deleted");
	
	if (enableValidationLayers) {
		instance.destroyDebugUtilsMessengerEXT(debugUtilsMessenger, nullptr);
		PSIM_CORE_INFO("Debug Messenger deleted");
	}

	instance.destroySurfaceKHR(surface, nullptr);
	PSIM_CORE_INFO("Surface deleted");

	instance.destroy(nullptr);
	PSIM_CORE_INFO("Vulkan instance deleted");

	glfwDestroyWindow(window);

	glfwTerminate();
	PSIM_CORE_INFO("Glfw terminated");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Instance Creation
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createInstance()
{
	PSIM_CORE_INFO("Initializing Vulkan instance and extensions");
	//creating info for vk instances
	vk::ApplicationInfo appInfo = { "Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };

	//setting extensions/ debugging instance creation/destruction
	//getting extensions/layers
	std::vector<const char*> extensions = getInstanceExtensions();
	std::vector<const char*> vLayers = {};
	if (enableValidationLayers) {
		vLayers = getInstanceLayers();
		appInfo.setPNext(&(populateDebugMessengerCreateInfo()));
	}

	//creating the instance
	auto [result, tempInstance] = vk::createInstance(vk::InstanceCreateInfo{ {}, &appInfo, static_cast<uint32_t>(vLayers.size()), vLayers.data(), static_cast<uint32_t>(extensions.size()), extensions.data() }, nullptr);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to create Vulkan instance!");
	instance = tempInstance;
	PSIM_CORE_INFO("Vulkan Instance Created");
}

std::vector<const char*> VkRender::getInstanceExtensions()
{
	//required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//add extra reqs here
	if (enableValidationLayers) {
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	auto [result, availableExtensions] = vk::enumerateInstanceExtensionProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate instance extension properties!");

	//compare required extensions to supported
	bool allFound = 1;
	for (auto i = 0; i < instanceExtensions.size(); i++) {
		bool found = false;

		for (const auto& extension : availableExtensions) {
			if (strcmp(instanceExtensions[i], extension.extensionName)) {
				PSIM_CORE_INFO("Added extension: {0}", instanceExtensions[i]);
				found = true;
				break;
			}
		}

		//error message
		PSIM_ASSERT(allFound, "Extension: {0} not found!", instanceExtensions[i]);
	}

	//success message
	PSIM_CORE_INFO("All required extensions found");

	return instanceExtensions;
}

std::vector<const char*> VkRender::getInstanceLayers()
{
	auto [result, availableLayers] = vk::enumerateInstanceLayerProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate instance layer properties!");

	//checking if layers are available
	bool allFound = 1;
	for (const char* layerName : instanceLayers) {
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
	
	return instanceLayers;
}

//--------------------------------------------------------------------------------------------------------------------------------


//Debugging funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::setupDebugMessenger()
{
	//if nDebug setup debug messenger
	if (enableValidationLayers)
	{
		pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
		PSIM_ASSERT(pfnVkCreateDebugUtilsMessengerEXT, "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");

		pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
		PSIM_ASSERT(pfnVkDestroyDebugUtilsMessengerEXT, "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");

		auto[result, tempDebugUtilsMessenger] = instance.createDebugUtilsMessengerEXT(populateDebugMessengerCreateInfo());
		PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to set up debug messenger!");
		debugUtilsMessenger = tempDebugUtilsMessenger;
		PSIM_CORE_INFO("Debug Utils Messenger Created");
	}
}

vk::DebugUtilsMessengerCreateInfoEXT VkRender::populateDebugMessengerCreateInfo()
{
	//puts this info into the VkDebugger
	vk::DebugUtilsMessengerCreateInfoEXT createInfo = { {}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
	vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
	&debugCallback, nullptr };
	return createInfo;
}

//--------------------------------------------------------------------------------------------------------------------------------


//Physical Device Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createSurface()
{
	VkSurfaceKHR tempSurface;
	PSIM_ASSERT(glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &tempSurface) == VK_SUCCESS, "Failed to create window surface!");
	surface = tempSurface; 
	PSIM_ASSERT(bool(surface), "Created Window Surface");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Physical Device Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::pickPhysicalDevice()
{
	PSIM_CORE_INFO("Picking GPU");
	//get devices
	uint32_t deviceCount = 0;
	auto [result, physicalDevices] = instance.enumeratePhysicalDevices();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate physical devices!");

	//actually pick from available
	std::unordered_map<int, vk::PhysicalDevice> candidates;

	for (const auto& device : physicalDevices) {
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

bool VkRender::isDeviceSuitable(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

int VkRender::rateDevice(vk::PhysicalDevice device)
{
	//get features and properties
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
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

bool VkRender::checkDeviceExtensionSupport(vk::PhysicalDevice device)
{
	//get extensions for each device
	auto [result, availableExtensions] = device.enumerateDeviceExtensionProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate device extension properties!");

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	//check against wanted extensions to see if all are found
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

//--------------------------------------------------------------------------------------------------------------------------------


//Queue Funcs
//--------------------------------------------------------------------------------------------------------------------------------
VkRender::QueueFamilyIndices VkRender::findQueueFamilies(vk::PhysicalDevice device)
{
	//create the queue family indeces
	QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	//check for graphics families in the queue
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		vk::Bool32 presentSupport = false;
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

//--------------------------------------------------------------------------------------------------------------------------------


//Logical Device Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createLogicalDevice()
{
	//get families
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	//create queue info
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo = { {}, queueFamily, 1, &queuePriority };
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//create device info
	vk::PhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = true;

	vk::DeviceCreateInfo createInfo = { {}, static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), 0, nullptr, static_cast<uint32_t>(deviceExtensions.size()),
	deviceExtensions.data(), &deviceFeatures };

	if (enableValidationLayers) {
		createInfo.setEnabledLayerCount(static_cast<uint32_t>(instanceLayers.size()));
		createInfo.setPpEnabledLayerNames(instanceLayers.data());
	}

	//create device
	physicalDevice.createDevice(&createInfo, nullptr, &device);
	PSIM_ASSERT(bool(device), "Failed to create logical device!");
	PSIM_CORE_INFO("Created Logical Device");

	//get handles to the queues
	device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
	device.getQueue(indices.presentFamily.value(), 0, &presentQueue);
}

//--------------------------------------------------------------------------------------------------------------------------------


//Swapchain Funcs
//--------------------------------------------------------------------------------------------------------------------------------
VkRender::SwapChainSupportDetails VkRender::querySwapChainSupport(vk::PhysicalDevice device)
{
	auto [result, surfaceCapabilities] = device.getSurfaceCapabilitiesKHR(surface);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get surface capabilities!");
	//formats

	auto [result1, surfaceFormats] = device.getSurfaceFormatsKHR(surface);
	PSIM_ASSERT(result1 == vk::Result::eSuccess, "Failed to get surface formats!");

	//modes
	auto[result2, surfacePresentModes] = device.getSurfacePresentModesKHR(surface);
	PSIM_ASSERT(result2 == vk::Result::eSuccess, "Failed to get surface present modes!");

	SwapChainSupportDetails details(surfaceCapabilities, surfaceFormats, surfacePresentModes);
	return details;
}

vk::SurfaceFormatKHR VkRender::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	//check for srgb
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR VkRender::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	//check for mailbox mode
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VkRender::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	//wanted extent
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	//fallback extent
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		vk::Extent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VkRender::createSwapChain()
{
	//create/populate the swapChainSupport struct
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//define an image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//create/populate a vkSwapChainInfo struct
	vk::SwapchainCreateInfoKHR createInfo = { {}, surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
	vk::SharingMode::eExclusive, 0, nullptr, swapChainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, true, nullptr };

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		createInfo.setQueueFamilyIndexCount(2);
		createInfo.setPQueueFamilyIndices(queueFamilyIndices);
	}

	//create the swapchain
	device.createSwapchainKHR(&createInfo, nullptr, &swapChain);
	PSIM_ASSERT(bool(swapChain), "Failed to create swap chain!");
	PSIM_CORE_INFO("Created Swap Chain");

	//retrieving images
	auto [result, images] = device.getSwapchainImagesKHR(swapChain);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get swapChain Images!");
	swapChainImages = images;

	//storing vars for later
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void VkRender::recreateSwapChain()
{
	//check new window size and adapt
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	device.waitIdle();

	//clean swapchain specific resources
	cleanupSwapChain();


	//recreate new swapchain specific resources
	createSwapChain();
	//createSwapchainImageViews();
	//createRenderPass();
	//createGraphicsPipeline();
	//createDepthResources();
	//createFramebuffers();
	//createUniformBuffers();
	//createDescriptorPool();
	//createDescriptorSets();
	//createCommandBuffers();

	PSIM_CORE_INFO("SwapChain recreated!");
}

void VkRender::cleanupSwapChain()
{
	
	//clean swapcain specific resources
	//device.destroyImageView(depthImageView, nullptr);
	//device.destroyImage(depthImage, nullptr);
	//device.freeMemory(depthImageMemory, nullptr);

	//for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
	//	device.destroyFramebuffer(swapChainFramebuffers[i], nullptr);
	//}

	//device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	//PSIM_CORE_INFO("Command Buffers freed");

	//device.destroyPipeline(graphicsPipeline, nullptr);
	//device.destroyPipelineLayout(pipelineLayout, nullptr);
	//device.destroyRenderPass(renderPass, nullptr);
	//PSIM_CORE_INFO("Pipeline deleted");

	//for (size_t i = 0; i < swapChainImageViews.size(); i++) {
	//	device.destroyImageView(swapChainImageViews[i], nullptr);
	//}
	//PSIM_CORE_INFO("ImageViews deleted");

	device.destroySwapchainKHR(swapChain, nullptr);
	PSIM_CORE_INFO("Swapchain cleaned up");

	//for (size_t i = 0; i < swapChainImages.size(); i++) {
	//	device.destroyBuffer(uniformBuffers[i], nullptr);
	//	device.freeMemory(uniformBuffersMemory[i], nullptr);
	//}
	//PSIM_CORE_INFO("Uniform Buffers cleaned up");

	//device.destroyDescriptorPool(descriptorPool, nullptr);
	//PSIM_CORE_INFO("Descriptor Pool cleaned up");
}