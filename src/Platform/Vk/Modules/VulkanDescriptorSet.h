#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet();
	~VulkanDescriptorSet();

	vk::DescriptorSetLayout createDescriptorSetLayout();
	vk::DescriptorPool createDescriptorPool();
	std::vector<vk::DescriptorSet> VulkanDescriptorSet::createDescriptorSets();
};

