#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet();
	~VulkanDescriptorSet();

	vk::DescriptorSetLayout createDescriptorSetLayout(vk::Device& device);
	vk::DescriptorPool createDescriptorPool(std::vector<vk::Image>& swapchainImages, vk::Device& device);
	std::vector<vk::DescriptorSet> VulkanDescriptorSet::createDescriptorSets(std::vector<vk::Image>& swapchainImages, vk::DescriptorSetLayout& descriptorSetLayout, vk::DescriptorPool& descriptorPool, vk::Device& device, std::vector<vk::Buffer>& uniformBuffers, vk::Sampler& textureSampler, vk::ImageView& textureImageView);
};

