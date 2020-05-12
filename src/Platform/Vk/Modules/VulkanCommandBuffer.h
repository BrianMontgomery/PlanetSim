#pragma once
#include "vulkan/vulkan.hpp"

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer();
	~VulkanCommandBuffer();

	vk::CommandPool createCommandPool();
	std::vector<vk::CommandBuffer> VulkanCommandBuffer::createCommandBuffers();
	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer& commandBuffer);
};

