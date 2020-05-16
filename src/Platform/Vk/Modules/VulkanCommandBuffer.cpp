#include "PSIMPCH.h"
#include "VulkanCommandBuffer.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include "Platform/Vk/Modules/VulkanQueue.h"
#include "Platform/Vk/Modules/VulkanPipeline.h"


VulkanCommandBuffer::VulkanCommandBuffer()
{
}


VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

vk::CommandPool VulkanCommandBuffer::createCommandPool()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//find all available queue families
	VulkanQueue queueFinder;
	VulkanQueue::QueueFamilyIndices queueFamilyIndices = queueFinder.findQueueFamilies(framework->physicalDevice);

	//create a command pool
	vk::CommandPoolCreateInfo poolInfo = { {}, queueFamilyIndices.graphicsFamily.value() };

	vk::CommandPool commandPool;
	PSIM_ASSERT(framework->device.createCommandPool(&poolInfo, nullptr, &commandPool) == vk::Result::eSuccess, "Failed to create command pool!");
	PSIM_CORE_INFO("Command Pool created");

	return commandPool;
}

std::vector<vk::CommandBuffer> VulkanCommandBuffer::createCommandBuffers()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get number of framebuffers
	std::vector<vk::CommandBuffer> commandBuffers;
	commandBuffers.resize(framework->swapchainFramebuffers.size());

	//allocate command buffers
	vk::CommandBufferAllocateInfo allocInfo = { framework->commandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)commandBuffers.size() };

	PSIM_ASSERT(framework->device.allocateCommandBuffers(&allocInfo, commandBuffers.data()) == vk::Result::eSuccess, "Failed to allocate command buffers!");

	//define and use command buffers
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//define buffers
		vk::CommandBufferBeginInfo beginInfo = {};

		PSIM_ASSERT(commandBuffers[i].begin(&beginInfo) == vk::Result::eSuccess, "Failed to begin recording command buffer!");

		std::array<vk::ClearValue, 2> clearValues = {};
		clearValues[0].setColor(vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f }));
		clearValues[1].setDepthStencil(vk::ClearDepthStencilValue({ 1.0f, 0 }));

		VulkanRenderPassLibrary * renderPassLib = VulkanRenderPassLibrary::getRenderPassLibrary();
		vk::RenderPassBeginInfo renderPassInfo = { renderPassLib->get(0)->getrenderPass(), framework->swapchainFramebuffers[i], vk::Rect2D { { 0, 0 }, framework->swapchainExtent }, static_cast<uint32_t>(clearValues.size()), clearValues.data() };

		//use command buffers
		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
		//bind pipeline
		VulkanPipelineLibrary *pipelineLib = VulkanPipelineLibrary::getPipelineLibrary();
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipelineLib->get(framework->currentPipelineName)->getPipeline());

		vk::Buffer vertexBuffers[] = { framework->vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };

		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(framework->indexBuffer, 0, vk::IndexType::eUint32);
		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLib->get(framework->currentPipelineName)->getPipelineLayout(), 0, 1, &framework->descriptorSets[i], 0, nullptr);

		//draw from pipeline
		commandBuffers[i].drawIndexed(static_cast<uint32_t>(framework->indices.size()), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();

		PSIM_ASSERT(commandBuffers[i].end() == vk::Result::eSuccess, "Failed to record command buffer!");
	}
	PSIM_CORE_INFO("Command buffers created");

	return commandBuffers;
}

vk::CommandBuffer VulkanCommandBuffer::beginSingleTimeCommands()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//setup a command buffer to be synchronized
	vk::CommandBufferAllocateInfo allocInfo = { framework->commandPool, vk::CommandBufferLevel::ePrimary, 1 };

	vk::CommandBuffer commandBuffer;
	framework->device.allocateCommandBuffers(&allocInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

	commandBuffer.begin(&beginInfo);

	return commandBuffer;
}

void VulkanCommandBuffer::endSingleTimeCommands(vk::CommandBuffer& commandBuffer)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//submit and sync command buffers
	commandBuffer.end();

	vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 1, &commandBuffer };

	framework->graphicsQueue.submit(1, &submitInfo, vk::Fence());
	framework->graphicsQueue.waitIdle();

	framework->device.freeCommandBuffers(framework->commandPool, 1, &commandBuffer);
}