#pragma once
#include "vulkan\vulkan.hpp"

class VulkanMemory
{
public:
	VulkanMemory();
	~VulkanMemory();

	void copyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue);
	uint32_t findMemoryType(vk::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	void transitionImageLayout(vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue);
	void copyBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize& size, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice& physicalDevice);
};

