#include "PSIMPCH.h"
#include "VulkanDevice.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include "Platform/Vk/Modules/VulkanQueue.h"
#include "Platform/Vk/Modules/VulkanSwapchain.h"
#include <set>


VulkanDevice::VulkanDevice()
{
}


VulkanDevice::~VulkanDevice()
{
}

vk::PhysicalDevice VulkanDevice::pickPhysicalDevice()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	PSIM_CORE_INFO("Picking GPU");
	//get devices
	uint32_t deviceCount = 0;
	auto[result, physicalDevices] = framework->instance.enumeratePhysicalDevices();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate physical devices!");
	getDefaults();

	//actually pick from available
	std::unordered_map<int, vk::PhysicalDevice> candidates;

	for (const auto& testPhysicalDevice : physicalDevices) {
		if (isDeviceSuitable(testPhysicalDevice)) {
			int score = rateDevice(testPhysicalDevice);
			candidates.insert(std::make_pair(score, testPhysicalDevice));
		}
	}

	// Check if the best candidate is suitable at all
	PSIM_ASSERT(candidates.begin()->first > 0, "Failed to find a suitable GPU!");
	PSIM_CORE_INFO("GPU selected");
	return candidates.begin()->second;
}

void VulkanDevice::getDefaults()
{
	PSIM_PROFILE_FUNCTION();
	deviceExtensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
}

bool VulkanDevice::isDeviceSuitable(vk::PhysicalDevice testPhysicalDevice)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	VulkanQueue queue;
	VulkanQueue::QueueFamilyIndices indices = queue.findQueueFamilies(testPhysicalDevice);

	bool extensionsSupported = checkDeviceExtensionSupport(testPhysicalDevice);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		VulkanSwapchain swapchainFuncs;
		SwapChainSupportDetails swapChainSupport = swapchainFuncs.querySwapChainSupport(testPhysicalDevice);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	vk::PhysicalDeviceFeatures supportedFeatures = testPhysicalDevice.getFeatures();

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

int VulkanDevice::rateDevice(const vk::PhysicalDevice testPhysicalDevice)
{
	PSIM_PROFILE_FUNCTION();
	//get features and properties
	vk::PhysicalDeviceProperties deviceProperties = testPhysicalDevice.getProperties();
	vk::PhysicalDeviceFeatures deviceFeatures = testPhysicalDevice.getFeatures();

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

bool VulkanDevice::checkDeviceExtensionSupport(vk::PhysicalDevice& testPhysicalDevice)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get extensions for each device
	auto[result, availableExtensions] = testPhysicalDevice.enumerateDeviceExtensionProperties();
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to enumerate device extension properties!");

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	//check against wanted extensions to see if all are found
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

vk::Device VulkanDevice::createLogicalDevice(std::vector<const char*> instanceLayers)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get families
	VulkanQueue queue;
	VulkanQueue::QueueFamilyIndices indices = queue.findQueueFamilies(framework->physicalDevice);

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
	vk::Device device;
	PSIM_ASSERT(framework->physicalDevice.createDevice(&createInfo, nullptr, &device) == vk::Result::eSuccess, "Failed to create logical device!");
	PSIM_CORE_INFO("Created Logical Device");

	//get handles to the queues
	device.getQueue(indices.graphicsFamily.value(), 0, &framework->graphicsQueue);
	device.getQueue(indices.presentFamily.value(), 0, &framework->presentQueue);

	return device;
}