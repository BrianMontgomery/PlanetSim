#include "PSIMPCH.h"
#include "VulkanUniformBuffer.h"

#include "vulkan/vulkan.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Platform/Vk/Modules/VulkanBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer()
{
}


VulkanUniformBuffer::~VulkanUniformBuffer()
{
}

std::vector<vk::Buffer> VulkanUniformBuffer::createUniformBuffers(std::vector<vk::DeviceMemory>& uniformBuffersMemory, std::vector<vk::Image>& swapchainImages, vk::PhysicalDevice& physicalDevice, vk::Device& device)
{
	//get uniform buffer sizes
	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

	std::vector<vk::Buffer> uniformBuffers;
	uniformBuffers.resize(swapchainImages.size());
	uniformBuffersMemory.resize(swapchainImages.size());

	//create buffers
	VulkanBuffer buff;
	for (size_t i = 0; i < swapchainImages.size(); i++) {
		buff.createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i], physicalDevice, device);
	}

	return uniformBuffers;
}

void VulkanUniformBuffer::updateUniformBuffer(uint32_t currentImage, vk::Extent2D& swapchainExtent, vk::Device& device, std::vector<vk::DeviceMemory>& uniformBuffersMemory)
{
	//set clock for time
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	//set uniform bufferobject settings
	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), (time * glm::radians(90.0f)) / 8, glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	//store the uniformbuffer
	void* data;
	device.mapMemory(uniformBuffersMemory[currentImage], vk::DeviceSize(), sizeof(ubo), vk::MemoryMapFlags(), &data);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformBuffersMemory[currentImage]);
}