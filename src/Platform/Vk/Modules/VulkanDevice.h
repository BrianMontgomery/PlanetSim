#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDevice
{
public:
	VulkanDevice();
	~VulkanDevice();

	vk::PhysicalDevice pickPhysicalDevice();
	void getDefaults();
	bool isDeviceSuitable(vk::PhysicalDevice device);
	int rateDevice(vk::PhysicalDevice device);
	bool checkDeviceExtensionSupport(vk::PhysicalDevice& device);

	vk::Device createLogicalDevice(std::vector<const char*> instanceLayers);

private:
	std::vector<const char*> deviceExtensions;
};

