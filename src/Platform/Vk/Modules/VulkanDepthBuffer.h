#pragma once
#include "vulkan/vulkan.hpp"


class VulkanDepthBuffer
{
public:
	VulkanDepthBuffer();
	~VulkanDepthBuffer();

	vk::ImageView createDepthResources();
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	vk::Format findDepthFormat();
	bool hasStencilComponent(vk::Format format);
};

