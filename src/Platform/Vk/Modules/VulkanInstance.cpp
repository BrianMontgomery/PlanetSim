#include "PSIMPCH.h"
#include "VulkanInstance.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"


VulkanInstance::VulkanInstance()
{
	getDefaults();
}


VulkanInstance::~VulkanInstance()
{
}

vk::Instance VulkanInstance::createNewInstance()
{
	PSIM_PROFILE_FUNCTION();
	PSIM_CORE_INFO("Initializing Vulkan instance and extensions");

	//creating the instance
	auto[result, tempInstance] = vk::createInstance(vk::InstanceCreateInfo{ {}, &appInfo, static_cast<uint32_t>(instanceLayers.size()), instanceLayers.data(), static_cast<uint32_t>(instanceExtensions.size()), instanceExtensions.data() }, nullptr);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to create Vulkan instance!");
	return tempInstance;
	PSIM_CORE_INFO("Vulkan Instance Created");
}

void VulkanInstance::getDefaults()
{
	PSIM_PROFILE_FUNCTION();
	//appInfo
	//creating info for vk instances
	appInfo = { "Default Name", VK_MAKE_VERSION(1, 0, 0), "PSIM Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };

	//extensions
	//required glfw extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> glfwXtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	instanceExtensions = glfwXtensions;

	//add extra reqs here
	if (enableValidationLayers) {
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	//compare required extensions to supported
	for (auto i = 0; i < instanceExtensions.size(); i++) {
		PSIM_ASSERT(checkAvailableExtensions(instanceExtensions[i]), "Extension {0} addition: failed!", instanceExtensions[i]);
	}

	//success message
	PSIM_CORE_INFO("All default extensions found");

	//layers
	addLayer("VK_LAYER_KHRONOS_validation");

	//success message
	PSIM_CORE_INFO("All default layers found");
}

bool VulkanInstance::checkAvailableExtensions(const char* extension)
{
	PSIM_PROFILE_FUNCTION();
	//get available
	auto[result, availableExtensions] = vk::enumerateInstanceExtensionProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate instance extension properties!");

	//check
	for (const auto& aExtension : availableExtensions) {
		if (strcmp(extension, aExtension.extensionName)) {
			PSIM_CORE_INFO("Added extension: {0}", extension);
			return true;
		}
	}

	//if not found
	return false;
}

bool VulkanInstance::checkAvailableLayers(const char* layer)
{
	PSIM_PROFILE_FUNCTION();
	//get available layers
	auto[result, availableLayers] = vk::enumerateInstanceLayerProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate instance layer properties!");

	for (const auto& layerProperties : availableLayers) {
		if (strcmp(layer, layerProperties.layerName) == 0) {
			PSIM_CORE_INFO("Added layer: {0}", layer);
			return true;
		}
	}

	//if not found
	return false;
}

//for any additional extensions to be added by the application
void VulkanInstance::addExtension(const char* extension)
{
	PSIM_PROFILE_FUNCTION();
	PSIM_ASSERT(checkAvailableExtensions(extension), "Extension {0} addition: failed!", extension);

	instanceExtensions.push_back(extension);
	PSIM_INFO("Extension {0} addition: successful!", extension);
	return;
}

void VulkanInstance::addLayer(const char* layer)
{
	PSIM_PROFILE_FUNCTION();
	PSIM_ASSERT(checkAvailableLayers(layer), "Layer {0} addition: failed!", layer);

	instanceLayers.push_back(layer);
	PSIM_INFO("Layer {0} addition: successful!", layer);
	return;
}

/// Set the name of the application.
void VulkanInstance::setApplicationName(const char* pApplicationName_)
{
	PSIM_PROFILE_FUNCTION();
	appInfo.pApplicationName = pApplicationName_;
	return;
}

/// Set the version of the application.
void VulkanInstance::setApplicationVersion(uint32_t applicationVersion_)
{
	PSIM_PROFILE_FUNCTION();
	appInfo.applicationVersion = applicationVersion_;
	return;
}

/// Set the name of the engine.
void VulkanInstance::setEngineName(const char* pEngineName_)
{
	PSIM_PROFILE_FUNCTION();
	appInfo.pEngineName = pEngineName_;
	return;
}

/// Set the version of the engine.
void VulkanInstance::setEngineVersion(uint32_t engineVersion_)
{
	PSIM_PROFILE_FUNCTION();
	appInfo.engineVersion = engineVersion_;
	return;
}

/// Set the version of the api.
void VulkanInstance::setApiVersion(uint32_t apiVersion_)
{
	PSIM_PROFILE_FUNCTION();
	appInfo.apiVersion = apiVersion_;
	return;
}

VulkanSurface::VulkanSurface()
{
}

VulkanSurface::~VulkanSurface()
{
}

vk::SurfaceKHR VulkanSurface::createNewSurface(GLFWwindow *window)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	VkSurfaceKHR tempSurface;
	PSIM_ASSERT(glfwCreateWindowSurface(VkInstance(framework->instance), window, nullptr, &tempSurface) == VK_SUCCESS, "Failed to create window surface!");
	PSIM_CORE_INFO("SurfaceKHR Created");
	return tempSurface;
}