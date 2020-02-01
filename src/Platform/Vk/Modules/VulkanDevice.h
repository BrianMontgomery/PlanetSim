#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDevice
{
public:
	VulkanDevice();
	~VulkanDevice();

	vk::PhysicalDevice pickPhysicalDevice(vk::Instance& instance, vk::SurfaceKHR& surface);
	void getDefaults();
	bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR& surface);
	int rateDevice(vk::PhysicalDevice device);
	bool checkDeviceExtensionSupport(vk::PhysicalDevice& device);

	vk::Device createLogicalDevice(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, std::vector<const char*> instanceLayers, vk::Queue &graphicsQueue, vk::Queue &presentQueue);

private:
	std::vector<const char*> deviceExtensions;
};

