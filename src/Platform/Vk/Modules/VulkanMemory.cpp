#include "PSIMPCH.h"
#include "VulkanMemory.h"

#include "Platform/Vk/Modules/VulkanCommandBuffer.h"


VulkanMemory::VulkanMemory()
{
}


VulkanMemory::~VulkanMemory()
{
}

void VulkanMemory::transitionImageLayout(vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue)
{
	PSIM_PROFILE_FUNCTION();
	VulkanCommandBuffer buff;

	//set up fences and semaphores for when wanted
	vk::CommandBuffer commandBuffer = buff.beginSingleTimeCommands(commandPool, device);

	//define the barrier
	vk::ImageMemoryBarrier barrier = { vk::AccessFlags(), vk::AccessFlags(), oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
		vk::ImageSubresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

	//set flags
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.setSrcAccessMask(vk::AccessFlags());
		barrier.setDstAccessMask(vk::AccessFlags(vk::AccessFlagBits::eTransferWrite));

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.setSrcAccessMask(vk::AccessFlags(vk::AccessFlagBits::eTransferWrite));
		barrier.setDstAccessMask(vk::AccessFlags(vk::AccessFlagBits::eShaderRead));

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		PSIM_CORE_ERROR("Unsupported layout transition!");
	}

	//set pipeline barrier
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	buff.endSingleTimeCommands(commandBuffer, graphicsQueue, commandPool, device);
}

void VulkanMemory::copyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue)
{
	PSIM_PROFILE_FUNCTION();
	VulkanCommandBuffer buff;

	vk::CommandBuffer commandBuffer = buff.beginSingleTimeCommands(commandPool, device);

	//get buffer region to copy and its info
	vk::BufferImageCopy region = { 0, 0, 0, vk::ImageSubresourceLayers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 }, { 0, 0, 0 }, { width, height, 1 } };

	//copy data
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	buff.endSingleTimeCommands(commandBuffer, graphicsQueue, commandPool, device);
}

uint32_t VulkanMemory::findMemoryType(vk::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	PSIM_PROFILE_FUNCTION();
	//get memory props
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	PSIM_CORE_ERROR("Failed to find suitable memory type!");
	return 0;
}

void VulkanMemory::copyBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize& size, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue)
{
	PSIM_PROFILE_FUNCTION();
	VulkanCommandBuffer buff;

	//copy one buffer to another
	vk::CommandBuffer commandBuffer = buff.beginSingleTimeCommands(commandPool, device);

	vk::BufferCopy copyRegion = { 0, 0, size };
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	buff.endSingleTimeCommands(commandBuffer, graphicsQueue, commandPool, device);
}

uint32_t VulkanMemory::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice& physicalDevice)
{
	PSIM_PROFILE_FUNCTION();
	//get memory props
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	PSIM_CORE_ERROR("Failed to find suitable memory type!");
	return 0;
}