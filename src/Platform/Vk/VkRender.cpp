/*
CONFIGURABLE VARIABLES - WHERE TO FIND
	texture/model paths - global
	extra required debug extensions for debugging - getInstanceExtensions()
	debug message severity controls - populateDebugMessengerCreateInfo() and debugCallback()
	validation layers - this.h
	required device supported extensions - this.h
	render pass settings - renderPass()
	shader code location and files - createGraphicsPipeline() (top)
	shader and pipeline info - createGraphicsPipeline()
	shader attribute bindings - this.h
	wanted depth formats - findDepthFormat()
	wanted stencil attributes - has stencil component
	images and texture - texture funcs
	texture sampler info - createTextureSampler()
	descriptor sets - createDescriptorSetLayout()
	ubo settings - updateUniformBuffer()
	descriptor set settings - createDescriptorSets()
	drawing and submission settings - drawFrame()
	max frames in flight - this.h
	sync object settings - createSyncObjects()
*/

//core files
#include "PSIMPCH.h"

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
	createSwapchainImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandPool();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();

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
	drawFrame();

	return (glfwWindowShouldClose(window));
}

void VkRender::cleanUp()
{
	device.waitIdle();
	
	//final cleanup of Vulkan resources
	cleanupSwapChain();

	device.destroySampler(textureSampler, nullptr);
	PSIM_CORE_INFO("Texture sampler destroyed");

	device.destroyImageView(textureImageView, nullptr);
	PSIM_CORE_INFO("Texture image view destroyed");

	device.destroyImage(textureImage, nullptr);
	device.freeMemory(textureImageMemory, nullptr);
	PSIM_CORE_INFO("Image destroyed");

	device.destroyDescriptorSetLayout(descriptorSetLayout, nullptr);

	device.destroyBuffer(indexBuffer, nullptr);
	device.freeMemory(indexBufferMemory, nullptr);
	PSIM_CORE_INFO("Index Buffer Destroyed");

	device.destroyBuffer(vertexBuffer, nullptr);
	device.freeMemory(vertexBufferMemory, nullptr);
	PSIM_CORE_INFO("Vertex Buffer Destroyed");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		device.destroySemaphore(renderFinishedSemaphores[i], nullptr);
		device.destroySemaphore(imageAvailableSemaphores[i], nullptr);
		device.destroyFence(inFlightFences[i], nullptr);
	}
	PSIM_CORE_INFO("Sync Objects deleted");

	device.destroyCommandPool(commandPool, nullptr);
	PSIM_CORE_INFO("Command Pool deleted");
	
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
	PSIM_ASSERT(physicalDevice.createDevice(&createInfo, nullptr, &device) == vk::Result::eSuccess, "Failed to create logical device!");
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
	PSIM_ASSERT(device.createSwapchainKHR(&createInfo, nullptr, &swapChain) == vk::Result::eSuccess, "Failed to create swap chain!");
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
	createSwapchainImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();

	PSIM_CORE_INFO("SwapChain recreated!");
}

void VkRender::cleanupSwapChain()
{
	
	//clean swapcain specific resources
	device.destroyImageView(depthImageView, nullptr);
	device.destroyImage(depthImage, nullptr);
	device.freeMemory(depthImageMemory, nullptr);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		device.destroyFramebuffer(swapChainFramebuffers[i], nullptr);
	}

	device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	PSIM_CORE_INFO("Command Buffers freed");

	device.destroyPipeline(graphicsPipeline, nullptr);
	device.destroyPipelineLayout(pipelineLayout, nullptr);
	device.destroyRenderPass(renderPass, nullptr);
	PSIM_CORE_INFO("Pipeline deleted");

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		device.destroyImageView(swapChainImageViews[i], nullptr);
	}
	PSIM_CORE_INFO("ImageViews deleted");

	device.destroySwapchainKHR(swapChain, nullptr);
	PSIM_CORE_INFO("Swapchain cleaned up");

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		device.destroyBuffer(uniformBuffers[i], nullptr);
		device.freeMemory(uniformBuffersMemory[i], nullptr);
	}
	PSIM_CORE_INFO("Uniform Buffers cleaned up");

	device.destroyDescriptorPool(descriptorPool, nullptr);
	PSIM_CORE_INFO("Descriptor Pool cleaned up");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Image View Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createSwapchainImageViews()
{
	//get number of swapchain image views needed
	swapChainImageViews.resize(swapChainImages.size());

	//create them
	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, vk::ImageAspectFlagBits::eColor);
	}
	PSIM_CORE_INFO("Created swapchain image views");
}

vk::ImageView VkRender::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
	//define image view
	vk::ImageViewCreateInfo viewInfo = { {}, image, vk::ImageViewType::e2D, format, {}, {aspectFlags, 0, 1, 0, 1} };

	//create it
	vk::ImageView imageView;
	PSIM_ASSERT(device.createImageView(&viewInfo, nullptr, &imageView) == vk::Result::eSuccess, "Failed to create texture image view!");

	return imageView;
}

//--------------------------------------------------------------------------------------------------------------------------------


//Graphics Pipeline Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createRenderPass()
{
	//color attachment
	vk::AttachmentDescription colorAttachment = { {}, swapChainImageFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR };

	//depth attachment
	vk::AttachmentDescription depthAttachment = { {}, findDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, 
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal };

	//color attachment reference
	vk::AttachmentReference colorAttachmentRef = {0, vk::ImageLayout::eColorAttachmentOptimal };

	//depth attachment reference
	vk::AttachmentReference depthAttachmentRef = { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

	//subpass
	vk::SubpassDescription subpass = { {}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef, nullptr, &depthAttachmentRef, 0, nullptr};

	//dependencies
	vk::SubpassDependency dependency = { VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
	vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, {} };

	//create render pass
	std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	vk::RenderPassCreateInfo renderPassInfo = { {}, static_cast<uint32_t>(attachments.size()), attachments.data(), 1, &subpass, 1, &dependency };

	PSIM_ASSERT(device.createRenderPass(&renderPassInfo, nullptr, &renderPass) == vk::Result::eSuccess, "Failed to create render pass!");
}

void VkRender::createGraphicsPipeline()
{
	auto vertShaderCode = readFileByteCode("src/Platform/Vk/Shaders/TriangleShaderVert.spv");
	auto fragShaderCode = readFileByteCode("src/Platform/Vk/Shaders/TriangleShaderFrag.spv");

	vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	//Vertex SHader Stage Info
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {{}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main"};

	//Fragment Shader Stage Info
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Vertex Input Stage Info
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, 1, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data() };

	//Input Assembly Stage Info
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, false };

	//Viewport info
	vk::Viewport viewport = {0.0f, 0.0f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };

	//Scissor Info
	vk::Rect2D scissor = { { 0, 0 }, swapChainExtent };

	//Viewport State Info
	vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

	//Rasterizer Info
	vk::PipelineRasterizationStateCreateInfo rasterizer = { {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, false, 0.0f, 0.0f, 0.0f, 1.0f };

	//MultiSampling Info
	vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false };

	vk::PipelineDepthStencilStateCreateInfo depthStencil = { {}, true, true, vk::CompareOp::eLess, false, false };

	//Color Blending Info
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = { true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, 
	vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

	//Color Blend State Info
	vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment };

	//Dynamic State Info
	vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

	vk::PipelineDynamicStateCreateInfo dynamicState = { {}, 2, dynamicStates };

	//Pipeline Layout Info
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetLayout };

	//create pipeline layout
	PSIM_ASSERT(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) == vk::Result::eSuccess, "Failed to create pipeline layout!");

	//creating the pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil, 
	&colorBlending, nullptr, pipelineLayout, renderPass, 0 };

	PSIM_ASSERT(device.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &graphicsPipeline) == vk::Result::eSuccess, "Failed to create graphics pipeline!");
	PSIM_CORE_INFO("Created Graphics Pipeline");

	//cleanup
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

vk::ShaderModule VkRender::createShaderModule(const std::vector<char>& code)
{
	//shader module  info
	vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

	//create it
	vk::ShaderModule shaderModule;
	PSIM_ASSERT(device.createShaderModule(&createInfo, nullptr, &shaderModule) == vk::Result::eSuccess, "Failed to create shader module!");

	return shaderModule;
}

//--------------------------------------------------------------------------------------------------------------------------------


//Frame Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createFramebuffers()
{
	//get number of framebuffers
	swapChainFramebuffers.resize(swapChainImageViews.size());

	//create framebuffers
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			swapChainImageViews[i],
			depthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = { {}, renderPass, static_cast<uint32_t>(attachments.size()), attachments.data(), swapChainExtent.width, swapChainExtent.height, 1 };

		PSIM_ASSERT(device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]) == vk::Result::eSuccess, "Failed to create framebuffer!");
	}
	PSIM_CORE_INFO("Framebuffers created");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Command Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createCommandPool()
{
	//find all available queue families
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	//create a command pool
	vk::CommandPoolCreateInfo poolInfo = { {}, queueFamilyIndices.graphicsFamily.value() };

	PSIM_ASSERT(device.createCommandPool(&poolInfo, nullptr, &commandPool) == vk::Result::eSuccess, "Failed to create command pool!");
	PSIM_CORE_INFO("Command Pool created");
}

void VkRender::createCommandBuffers()
{
	//get number of framebuffers
	commandBuffers.resize(swapChainFramebuffers.size());

	//allocate command buffers
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)commandBuffers.size() };

	PSIM_ASSERT(device.allocateCommandBuffers(&allocInfo, commandBuffers.data()) == vk::Result::eSuccess, "Failed to allocate command buffers!");

	//define and use command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//define buffers
		vk::CommandBufferBeginInfo beginInfo = {};

		PSIM_ASSERT(commandBuffers[i].begin(&beginInfo) == vk::Result::eSuccess, "Failed to begin recording command buffer!");

		std::array<vk::ClearValue, 2> clearValues = {};
		clearValues[0].setColor(vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f }));
		clearValues[1].setDepthStencil(vk::ClearDepthStencilValue({1.0f, 0 }));

		vk::RenderPassBeginInfo renderPassInfo = { renderPass, swapChainFramebuffers[i], vk::Rect2D { { 0, 0 }, swapChainExtent }, static_cast<uint32_t>(clearValues.size()), clearValues.data() };

		//use command buffers
		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
		//bind pipeline
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };

		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		//draw from pipeline
		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();
		
		PSIM_ASSERT(commandBuffers[i].end() == vk::Result::eSuccess, "Failed to record command buffer!");
	}
	PSIM_CORE_INFO("Command buffers created");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Depth Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createDepthResources()
{
	//get depth format
	vk::Format depthFormat = findDepthFormat();

	//create depth resources
	createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, 
		vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
	depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format VkRender::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	//check for optimal formats
	for (vk::Format format : candidates) {
		vk::FormatProperties props;
		physicalDevice.getFormatProperties(format, &props);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	PSIM_CORE_ERROR("failed to find supported format!");
}

vk::Format VkRender::findDepthFormat()
{
	//return the best format here
	return findSupportedFormat( { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment );
}

bool VkRender::hasStencilComponent(vk::Format format)
{
	//check for this stencil component
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

//--------------------------------------------------------------------------------------------------------------------------------


//Texture funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createTextureImage()
{
	//load texture
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(TEXTURE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	PSIM_ASSERT(pixels, "Failed to load texture image!");

	//designate a buffer for the texture
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	//transfer texture data to new buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	//unload image
	stbi_image_free(pixels);

	//create image
	createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

	//transition data format to optimal
	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	//free unneeded resources
	PSIM_CORE_INFO("Created texture");
	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);
}

void VkRender::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) 
{
	//define an image 
	vk::ImageCreateInfo imageInfo = { {}, vk::ImageType::e2D, format, vk::Extent3D { width, height, 1 }, 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive };

	//create image
	PSIM_ASSERT(device.createImage(&imageInfo, nullptr, &image) == vk::Result::eSuccess, "Failed to create image!");

	//allocate memory
	vk::MemoryRequirements memRequirements;
	device.getImageMemoryRequirements(image, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(device.allocateMemory(&allocInfo, nullptr, &imageMemory) == vk::Result::eSuccess, "Failed to allocate image memory!");

	//bind image to an image memory object
	device.bindImageMemory(image, imageMemory, 0);
}

void VkRender::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) 
{
	//set up fences and semaphores for when wanted
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	//define the barrier
	vk::ImageMemoryBarrier barrier = { vk::AccessFlags(), vk::AccessFlags(), oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, 
		vk::ImageSubresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

	//set flags
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.setSrcAccessMask(vk::AccessFlags());
		barrier.setDstAccessMask(vk::AccessFlags(vk::AccessFlagBits::eTransferWrite));

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.setSrcAccessMask(vk::AccessFlags(vk::AccessFlagBits::eTransferWrite));
		barrier.setDstAccessMask(vk::AccessFlags(vk::AccessFlagBits::eShaderRead));

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		PSIM_CORE_ERROR("Unsupported layout transition!");
	}

	//set pipeline barrier
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void VkRender::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) 
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	//get buffer region to copy and its info
	vk::BufferImageCopy region = { 0, 0, 0, vk::ImageSubresourceLayers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 }, { 0, 0, 0 }, { width, height, 1 } };

	//copy data
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

void VkRender::createTextureImageView()
{
	//create tex image view
	textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
	PSIM_CORE_INFO("Created Texture image view");
}

void VkRender::createTextureSampler()
{
	//define and create texture sampler
	vk::SamplerCreateInfo samplerInfo = { {}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 
	vk::SamplerAddressMode::eRepeat, 0.0f, true, 16, false, vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, false };

	PSIM_ASSERT(device.createSampler(&samplerInfo, nullptr, &textureSampler) == vk::Result::eSuccess, "Failed to create texture sampler!");
}

//--------------------------------------------------------------------------------------------------------------------------------


//Model Loading Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::loadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	//load model
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
		PSIM_CORE_ERROR(warn + err);
	}

	//ensure that each vertex is unique and parsed according to obj rulesu
	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------


//Vertex Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createVertexBuffer()
{
	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	//fill buffer
	void* data;
	device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);
}

void VkRender::createIndexBuffer()
{
	//get buffer size
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	//fill buffer
	void* data;
	device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);
}

void VkRender::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
	//define and create buffer
	vk::BufferCreateInfo bufferInfo = { {}, size, usage,vk::SharingMode::eExclusive };

	
	PSIM_ASSERT(device.createBuffer(&bufferInfo, nullptr, &buffer) == vk::Result::eSuccess, "Failed to create buffer!");

	//allocate buffer memory
	vk::MemoryRequirements memRequirements;
	device.getBufferMemoryRequirements(buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(device.allocateMemory(&allocInfo, nullptr, &bufferMemory) == vk::Result::eSuccess, "Failed to allocate buffer memory!");

	device.bindBufferMemory(buffer, bufferMemory, 0);
}

uint32_t VkRender::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	//get memory props
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	PSIM_CORE_ERROR("Failed to find suitable memory type!");
}

vk::CommandBuffer VkRender::beginSingleTimeCommands()
{
	//setup a command buffer to be synchronized
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };

	vk::CommandBuffer commandBuffer;
	device.allocateCommandBuffers(&allocInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(&beginInfo);

	return commandBuffer;
}

void VkRender::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	//submit and sync command buffers
	commandBuffer.end();

	vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

	graphicsQueue.submit(1, &submitInfo, vk::Fence());
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void VkRender::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	//copy one buffer to another
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferCopy copyRegion = { 0, 0, size };
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

//--------------------------------------------------------------------------------------------------------------------------------


//uniform buffer funcs
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::createDescriptorSetLayout()
{
	//define the descriptor set layout
	vk::DescriptorSetLayoutBinding uboLayoutBinding = { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };

	vk::DescriptorSetLayoutBinding samplerLayoutBinding = { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, static_cast<uint32_t>(bindings.size()), bindings.data() };

	PSIM_ASSERT(device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout) == vk::Result::eSuccess, "Failed to create descriptor set layout!");
}

void VkRender::createUniformBuffers()
{
	//get uniform buffer sizes
	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	//create buffers
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void VkRender::updateUniformBuffer(uint32_t currentImage)
{
	//set clock for time
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	//set uniform bufferobject settings
	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), (time * glm::radians(90.0f)) / 8, glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	//store the uniformbuffer
	void* data;
	device.mapMemory(uniformBuffersMemory[currentImage], vk::DeviceSize(), sizeof(ubo), vk::MemoryMapFlags(), &data);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformBuffersMemory[currentImage]);
}

void VkRender::createDescriptorPool()
{
	//set both descriptor pool sizes
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0] = { vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(swapChainImages.size()) };
	poolSizes[1] = { vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(swapChainImages.size()) };

	//create descriptor pool
	vk::DescriptorPoolCreateInfo poolInfo = { {}, static_cast<uint32_t>(swapChainImages.size()), static_cast<uint32_t>(poolSizes.size()), poolSizes.data() };

	PSIM_ASSERT(device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool) == vk::Result::eSuccess, "Failed to create descriptor pool!");
}

void VkRender::createDescriptorSets()
{
	//get number of vectors
	std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);

	//allocate descriptor sets
	vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, static_cast<uint32_t>(swapChainImages.size()), layouts.data() };

	descriptorSets.resize(swapChainImages.size());
	PSIM_ASSERT(device.allocateDescriptorSets(&allocInfo, descriptorSets.data()) == vk::Result::eSuccess, "Failed to allocate descriptor sets!");

	//create descriptor sets
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		//get buffer info
		vk::DescriptorBufferInfo bufferInfo = { uniformBuffers[i], 0, sizeof(UniformBufferObject) };

		//get image info
		vk::DescriptorImageInfo imageInfo = { textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal};

		//set descriptor settings
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0] = { descriptorSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo };

		descriptorWrites[1] = { descriptorSets[i], 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo };

		device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------


//Rendering/Presentation
//--------------------------------------------------------------------------------------------------------------------------------
void VkRender::drawFrame()
{
	//ensure that all previous draws are completed
	device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//get next frame
	uint32_t imageIndex;
	vk::Result result = device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], vk::Fence(), &imageIndex);

	//check for resize
	if (result == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		return;
	}
	else {
		PSIM_ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "Failed to acquire swap chain image!");
	}

	//update to next frame
	updateUniformBuffer(imageIndex);

	if (bool(imagesInFlight[imageIndex]) != false) {
		device.waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//setup frame submission info

	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	vk::SubmitInfo submitInfo = { 1, waitSemaphores, waitStages, 1, &commandBuffers[imageIndex], 1, signalSemaphores };

	//reset sync objects when complete
	device.resetFences(1, &inFlightFences[currentFrame]);

	//push completed frame
	PSIM_ASSERT(graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]) == vk::Result::eSuccess, "Failed to submit draw command buffer!");

	//info for frame submission
	vk::SwapchainKHR swapChains[] = { swapChain };
	vk::PresentInfoKHR presentInfo = { 1, signalSemaphores, 1, swapChains, &imageIndex };

	//present frame
	result = presentQueue.presentKHR(&presentInfo);

	//check if frame was presented
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else {
		PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to present swap chain image!");
	}

	//get next frame
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VkRender::createSyncObjects()
{
	//get sync objects for current frames and resize
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size());

	//reset sync objects info
	vk::SemaphoreCreateInfo semaphoreInfo = {};

	vk::FenceCreateInfo fenceInfo = { vk::FenceCreateFlagBits::eSignaled};

	//recreate sync objects
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		PSIM_ASSERT(device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) == vk::Result::eSuccess &&
			device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) == vk::Result::eSuccess &&
			device.createFence(&fenceInfo, nullptr, &inFlightFences[i]) == vk::Result::eSuccess, "Failed to create synchronization objects for a frame!");
	}

	PSIM_CORE_INFO("Sync Objects Created");
}