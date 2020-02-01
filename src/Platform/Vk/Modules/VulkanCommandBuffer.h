#pragma once
#include "vulkan/vulkan.hpp"

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer();
	~VulkanCommandBuffer();

	vk::CommandPool createCommandPool(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, vk::Device& device);
	std::vector<vk::CommandBuffer> VulkanCommandBuffer::createCommandBuffers(std::vector<vk::Framebuffer>& swapChainFramebuffers, vk::CommandPool& commandPool, vk::Device& device, vk::RenderPass& renderPass, vk::Extent2D& swapchainExtent, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::Buffer& vertexBuffer, vk::Buffer& indexBuffer, std::vector<vk::DescriptorSet>& descriptorSets, std::vector<uint32_t>& indices);
	vk::CommandBuffer beginSingleTimeCommands(vk::CommandPool& commandPool, vk::Device& device);
	void endSingleTimeCommands(vk::CommandBuffer& commandBuffer, vk::Queue& graphicsQueue, vk::CommandPool& commandPool, vk::Device& device);
};

