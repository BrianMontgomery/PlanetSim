#include "PSIMPCH.h"
#include "VulkanCommandBuffer.h"

#include "Platform/Vk/Modules/VulkanQueue.h"


VulkanCommandBuffer::VulkanCommandBuffer()
{
}


VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

vk::CommandPool VulkanCommandBuffer::createCommandPool(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//find all available queue families
	VulkanQueue queueFinder;
	VulkanQueue::QueueFamilyIndices queueFamilyIndices = queueFinder.findQueueFamilies(physicalDevice, surface);

	//create a command pool
	vk::CommandPoolCreateInfo poolInfo = { {}, queueFamilyIndices.graphicsFamily.value() };

	vk::CommandPool commandPool;
	PSIM_ASSERT(device.createCommandPool(&poolInfo, nullptr, &commandPool) == vk::Result::eSuccess, "Failed to create command pool!");
	PSIM_CORE_INFO("Command Pool created");

	return commandPool;
}

std::vector<vk::CommandBuffer> VulkanCommandBuffer::createCommandBuffers(std::vector<vk::Framebuffer>& swapChainFramebuffers, vk::CommandPool& commandPool, vk::Device& device, vk::RenderPass& renderPass, vk::Extent2D& swapchainExtent, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::Buffer& vertexBuffer, vk::Buffer& indexBuffer, std::vector<vk::DescriptorSet>& descriptorSets, std::vector<uint32_t>& indices)
{
	PSIM_PROFILE_FUNCTION();
	//get number of framebuffers
	std::vector<vk::CommandBuffer> commandBuffers;
	commandBuffers.resize(swapChainFramebuffers.size());

	//allocate command buffers
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)commandBuffers.size() };

	PSIM_ASSERT(device.allocateCommandBuffers(&allocInfo, commandBuffers.data()) == vk::Result::eSuccess, "Failed to allocate command buffers!");

	//define and use command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//define buffers
		vk::CommandBufferBeginInfo beginInfo = {};

		PSIM_ASSERT(commandBuffers[i].begin(&beginInfo) == vk::Result::eSuccess, "Failed to begin recording command buffer!");

		std::array<vk::ClearValue, 2> clearValues = {};
		clearValues[0].setColor(vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f }));
		clearValues[1].setDepthStencil(vk::ClearDepthStencilValue({ 1.0f, 0 }));

		vk::RenderPassBeginInfo renderPassInfo = { renderPass, swapChainFramebuffers[i], vk::Rect2D { { 0, 0 }, swapchainExtent }, static_cast<uint32_t>(clearValues.size()), clearValues.data() };

		//use command buffers
		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
		//bind pipeline
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };

		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		//draw from pipeline
		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();

		PSIM_ASSERT(commandBuffers[i].end() == vk::Result::eSuccess, "Failed to record command buffer!");
	}
	PSIM_CORE_INFO("Command buffers created");

	return commandBuffers;
}

vk::CommandBuffer VulkanCommandBuffer::beginSingleTimeCommands(vk::CommandPool& commandPool, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//setup a command buffer to be synchronized
	vk::CommandBufferAllocateInfo allocInfo = { commandPool, vk::CommandBufferLevel::ePrimary, 1 };

	vk::CommandBuffer commandBuffer;
	device.allocateCommandBuffers(&allocInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(&beginInfo);

	return commandBuffer;
}

void VulkanCommandBuffer::endSingleTimeCommands(vk::CommandBuffer& commandBuffer, vk::Queue& graphicsQueue, vk::CommandPool& commandPool, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//submit and sync command buffers
	commandBuffer.end();

	vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

	graphicsQueue.submit(1, &submitInfo, vk::Fence());
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}