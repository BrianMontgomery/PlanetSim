#include "PSIMPCH.h"
#include "VulkanBuffer.h"

#include "Platform/Vk/Modules/VulkanMemory.h"


VulkanBuffer::VulkanBuffer()
{
}


VulkanBuffer::~VulkanBuffer()
{
}

//Vertex Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
vk::Buffer VulkanBuffer::createVertexBuffer(std::vector<Vertex>& vertices, vk::DeviceMemory& vertexBufferMemory, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::CommandPool& commandPool, vk::Queue& graphicsQueue)
{
	PSIM_PROFILE_FUNCTION();
	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	
	
	if (bufferSize == 0) {
		Vertex vertex = {};

		vertex.pos = {
			1.0f, 0.0f, 0.0f
		};

		vertex.texCoord = {
			-0.5f, -0.5f
		};

		vertex.color = { 1.0f, 1.0f, 1.0f };

		vertices.push_back(vertex);

		bufferSize = sizeof(vertices[0]) * vertices.size();
	}
	
	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory, physicalDevice, device);

	//fill buffer
	void* data;
	device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	vk::Buffer vertexBuffer;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory, physicalDevice, device);

	VulkanMemory mem;
	mem.copyBuffer(stagingBuffer, vertexBuffer, bufferSize, commandPool, device, graphicsQueue);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);

	return vertexBuffer;
}

vk::Buffer VulkanBuffer::createIndexBuffer(vk::DeviceMemory& indexBufferMemory, vk::PhysicalDevice& physicalDevice, vk::Device& device, std::vector<uint32_t>& indices, vk::CommandPool& commandPool, vk::Queue& graphicsQueue)
{
	PSIM_PROFILE_FUNCTION();
	//get buffer size
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	if (bufferSize == 0) {
		indices = { 0 };
		bufferSize = sizeof(indices[0]) * indices.size();
	}

	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory, physicalDevice, device);

	//fill buffer
	void* data;
	device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);

	vk::Buffer indexBuffer;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory, physicalDevice, device);

	VulkanMemory mem;
	mem.copyBuffer(stagingBuffer, indexBuffer, bufferSize, commandPool, device, graphicsQueue);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);

	return indexBuffer;
}

void VulkanBuffer::createBuffer(vk::DeviceSize& size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory, vk::PhysicalDevice& physicalDevice, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	//define and create buffer
	vk::BufferCreateInfo bufferInfo = { {}, size, usage,vk::SharingMode::eExclusive };


	PSIM_ASSERT(device.createBuffer(&bufferInfo, nullptr, &buffer) == vk::Result::eSuccess, "Failed to create buffer!");

	//allocate buffer memory
	vk::MemoryRequirements memRequirements;
	device.getBufferMemoryRequirements(buffer, &memRequirements);

	VulkanMemory mem;
	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, mem.findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(device.allocateMemory(&allocInfo, nullptr, &bufferMemory) == vk::Result::eSuccess, "Failed to allocate buffer memory!");

	device.bindBufferMemory(buffer, bufferMemory, 0);
}