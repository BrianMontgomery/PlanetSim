#pragma once
#include "vulkan/vulkan.hpp"

class VulkanShader
{
public:
	VulkanShader();
	~VulkanShader();

	vk::ShaderModule createShaderModule(const std::string& filename, vk::Device& device);
};

