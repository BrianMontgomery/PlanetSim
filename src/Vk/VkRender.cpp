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
	//createSurface();
	//pickPhysicalDevice();
	//createLogicalDevice();
	//createSwapChain();
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
	/*vk::DeviceWaitIdle(device);

	//final cleanup of Vulkan resources
	cleanupSwapChain();

	vkDestroySampler(device, textureSampler, nullptr);
	PSIM_CORE_INFO("Texture sampler destroyed");

	vkDestroyImageView(device, textureImageView, nullptr);
	PSIM_CORE_INFO("Texture image view destroyed");

	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);
	PSIM_CORE_INFO("Image destroyed");

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);
	PSIM_CORE_INFO("Index Buffer Destroyed");

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	PSIM_CORE_INFO("Vertex Buffer Destroyed");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}
	PSIM_CORE_INFO("Sync Objects deleted");

	vkDestroyCommandPool(device, commandPool, nullptr);
	PSIM_CORE_INFO("Command Pool deleted");

	vkDestroyDevice(device, nullptr);
	PSIM_CORE_INFO("Device deleted");
	*/
	if (enableValidationLayers) {
		vkDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
		PSIM_CORE_INFO("Debug Messenger deleted");
	}

	//vkDestroySurfaceKHR(instance, surface, nullptr);
	//PSIM_CORE_INFO("Surface deleted");

	vkDestroyInstance(instance, nullptr);
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
	std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };
	
	auto [result, availableLayers] = vk::enumerateInstanceLayerProperties();

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

/*void VkRender::createSurface()
{
	VkSurfaceKHR tempSurface;
	PSIM_ASSERT(glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &tempSurface) == VK_SUCCESS, "Failed to create window surface!");
	surface = tempSurface; 
	PSIM_CORE_INFO("Created Window Surface");
}*/