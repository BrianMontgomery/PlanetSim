#pragma once
#include "vulkan/vulkan.hpp"
class VulkanImage
{
public:
	VulkanImage();
	~VulkanImage();

	vk::ImageView VulkanImage::createImageView(vk::Image& image, vk::Device& device, vk::Format format, vk::ImageAspectFlags aspectFlags);
	vk::Image VulkanImage::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
};

