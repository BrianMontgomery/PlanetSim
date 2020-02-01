#pragma once
#include "vulkan/vulkan.hpp"

class VulkanTexture
{
public:
	VulkanTexture();
	~VulkanTexture();

	vk::Image createTextureImage(const char* TEXTURE_PATH, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue, vk::PhysicalDevice& physicalDevice, vk::DeviceMemory& textureImageMemory);
	vk::ImageView createTextureImageView(vk::Image& textureImage, vk::Device& device);
	vk::Sampler createTextureSampler(vk::Device& device);
};

