#include "PSIMPCH.h"
#include "VulkanDescriptorSet.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include "Platform/Vk/Modules/VulkanUniformBuffer.h"


VulkanDescriptorSet::VulkanDescriptorSet()
{
}


VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

vk::DescriptorSetLayout VulkanDescriptorSet::createDescriptorSetLayout()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//define the descriptor set layout
	vk::DescriptorSetLayoutBinding uboLayoutBinding = { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex };

	vk::DescriptorSetLayoutBinding samplerLayoutBinding = { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo = { {}, static_cast<uint32_t>(bindings.size()), bindings.data() };

	vk::DescriptorSetLayout descriptorSetLayout;
	PSIM_ASSERT(framework->device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout) == vk::Result::eSuccess, "Failed to create descriptor set layout!");

	return descriptorSetLayout;
}



vk::DescriptorPool VulkanDescriptorSet::createDescriptorPool()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//set both descriptor pool sizes
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0] = { vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(framework->swapchainImages.size()) };
	poolSizes[1] = { vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(framework->swapchainImages.size()) };

	//create descriptor pool
	vk::DescriptorPoolCreateInfo poolInfo = { {}, static_cast<uint32_t>(framework->swapchainImages.size()), static_cast<uint32_t>(poolSizes.size()), poolSizes.data() };

	vk::DescriptorPool descriptorPool;
	PSIM_ASSERT(framework->device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool) == vk::Result::eSuccess, "Failed to create descriptor pool!");

	return descriptorPool;
}

std::vector<vk::DescriptorSet> VulkanDescriptorSet::createDescriptorSets()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get number of vectors
	std::vector<vk::DescriptorSetLayout> layouts(framework->swapchainImages.size(), framework->descriptorSetLayout);

	//allocate descriptor sets
	vk::DescriptorSetAllocateInfo allocInfo = { framework->descriptorPool, static_cast<uint32_t>(framework->swapchainImages.size()), layouts.data() };

	std::vector<vk::DescriptorSet> descriptorSets;
	descriptorSets.resize(framework->swapchainImages.size());
	PSIM_ASSERT(framework->device.allocateDescriptorSets(&allocInfo, descriptorSets.data()) == vk::Result::eSuccess, "Failed to allocate descriptor sets!");

	//create descriptor sets
	for (size_t i = 0; i < framework->swapchainImages.size(); i++) {
		//get buffer info
		vk::DescriptorBufferInfo bufferInfo = { framework->uniformBuffers[i], 0, sizeof(VulkanUniformBuffer::UniformBufferObject) };

		//get image info
		vk::DescriptorImageInfo imageInfo = { framework->textureSampler, framework->textureImageView, vk::ImageLayout::eShaderReadOnlyOptimal };

		//set descriptor settings
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0] = { descriptorSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo };

		descriptorWrites[1] = { descriptorSets[i], 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo };

		framework->device.updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	return descriptorSets;
}