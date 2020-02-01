#pragma once
#include "vulkan/vulkan.hpp"


class VulkanDepthBuffer
{
public:
	VulkanDepthBuffer();
	~VulkanDepthBuffer();

	vk::ImageView createDepthResources(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::Extent2D& swapchainExtent, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory);
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, vk::PhysicalDevice& physicalDevice);
	vk::Format findDepthFormat(vk::PhysicalDevice& physicalDevice);
	bool hasStencilComponent(vk::Format format);
};

