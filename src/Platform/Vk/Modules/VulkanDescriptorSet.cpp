#include "PSIMPCH.h"
#include "VulkanDescriptorSet.h"

#include "Platform/Vk/Modules/VulkanUniformBuffer.h"


VulkanDescriptorSet::VulkanDescriptorSet()
{
}


VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

vk::DescriptorSetLayout VulkanDescriptorSet::createDescriptorSetLayout(vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//define the descriptor set layout
	vk::DescriptorSetLayoutBinding uboLayoutBinding = { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };

	vk::DescriptorSetLayoutBinding samplerLayoutBinding = { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, static_cast<uint32_t>(bindings.size()), bindings.data() };

	vk::DescriptorSetLayout descriptorSetLayout;
	PSIM_ASSERT(device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout) == vk::Result::eSuccess, "Failed to create descriptor set layout!");

	return descriptorSetLayout;
}



vk::DescriptorPool VulkanDescriptorSet::createDescriptorPool(std::vector<vk::Image>& swapchainImages, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//set both descriptor pool sizes
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0] = { vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(swapchainImages.size()) };
	poolSizes[1] = { vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(swapchainImages.size()) };

	//create descriptor pool
	vk::DescriptorPoolCreateInfo poolInfo = { {}, static_cast<uint32_t>(swapchainImages.size()), static_cast<uint32_t>(poolSizes.size()), poolSizes.data() };

	vk::DescriptorPool descriptorPool;
	PSIM_ASSERT(device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool) == vk::Result::eSuccess, "Failed to create descriptor pool!");

	return descriptorPool;
}

std::vector<vk::DescriptorSet> VulkanDescriptorSet::createDescriptorSets(std::vector<vk::Image>& swapchainImages, vk::DescriptorSetLayout& descriptorSetLayout, vk::DescriptorPool& descriptorPool, vk::Device& device, std::vector<vk::Buffer>& uniformBuffers, vk::Sampler& textureSampler, vk::ImageView& textureImageView)
{
	PSIM_PROFILE_FUNCTION();
	//get number of vectors
	std::vector<vk::DescriptorSetLayout> layouts(swapchainImages.size(), descriptorSetLayout);

	//allocate descriptor sets
	vk::DescriptorSetAllocateInfo allocInfo = { descriptorPool, static_cast<uint32_t>(swapchainImages.size()), layouts.data() };

	std::vector<vk::DescriptorSet> descriptorSets;
	descriptorSets.resize(swapchainImages.size());
	PSIM_ASSERT(device.allocateDescriptorSets(&allocInfo, descriptorSets.data()) == vk::Result::eSuccess, "Failed to allocate descriptor sets!");

	//create descriptor sets
	for (size_t i = 0; i < swapchainImages.size(); i++) {
		//get buffer info
		vk::DescriptorBufferInfo bufferInfo = { uniformBuffers[i], 0, sizeof(VulkanUniformBuffer::UniformBufferObject) };

		//get image info
		vk::DescriptorImageInfo imageInfo = { textureSampler, textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };

		//set descriptor settings
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0] = { descriptorSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo };

		descriptorWrites[1] = { descriptorSets[i], 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo };

		device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return descriptorSets;
}