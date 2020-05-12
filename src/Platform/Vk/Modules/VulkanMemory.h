#pragma once
#include "vulkan\vulkan.hpp"

class VulkanMemory
{
public:
	VulkanMemory();
	~VulkanMemory();

	void copyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	void transitionImageLayout(vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize& size);
};

