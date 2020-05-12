#pragma once
#include "vulkan/vulkan.hpp"

class VulkanTexture
{
public:
	VulkanTexture();
	~VulkanTexture();

	vk::Image createTextureImage(const char* TEXTURE_PATH);
	vk::ImageView createTextureImageView();
	vk::Sampler createTextureSampler();
};

