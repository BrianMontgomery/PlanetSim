#pragma once
#include "vulkan/vulkan.hpp"

class VulkanPipeline
{
public:
	VulkanPipeline();
	~VulkanPipeline();

	vk::RenderPass createRenderPass(vk::Format swapchainImageFormat, vk::PhysicalDevice& physicalDevice, vk::Device& device);
	vk::Pipeline createGraphicsPipeline(vk::Extent2D& swapchainExtent, vk::Device& device, vk::RenderPass& renderPass, vk::DescriptorSetLayout& descriptorSetlayout);

	vk::PipelineLayout getPipelineLayout()
		{ return pipelineLayout; }

private:
	vk::PipelineLayout pipelineLayout;
};

