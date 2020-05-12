#include "PSIMPCH.h"
#include "VulkanBuffer.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include "Platform/Vk/Modules/VulkanMemory.h"


VulkanBuffer::VulkanBuffer()
{
}


VulkanBuffer::~VulkanBuffer()
{
}

//Vertex Buffer Funcs
//--------------------------------------------------------------------------------------------------------------------------------
vk::Buffer VulkanBuffer::createVertexBuffer()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(framework->vertices[0]) * framework->vertices.size();
	
	
	if (bufferSize == 0) {
		Vertex vertex = {};

		vertex.pos = {
			1.0f, 0.0f, 0.0f
		};

		vertex.texCoord = {
			-0.5f, -0.5f
		};

		vertex.color = { 1.0f, 1.0f, 1.0f };

		framework->vertices.push_back(vertex);

		bufferSize = sizeof(framework->vertices[0]) * framework->vertices.size();
	}
	
	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	//fill buffer
	void* data;
	framework->device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, framework->vertices.data(), (size_t)bufferSize);
	framework->device.unmapMemory(stagingBufferMemory);

	vk::Buffer vertexBuffer;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, framework->vertexBufferMemory);

	VulkanMemory mem;
	mem.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	framework->device.destroyBuffer(stagingBuffer, nullptr);
	framework->device.freeMemory(stagingBufferMemory, nullptr);

	return vertexBuffer;
}

vk::Buffer VulkanBuffer::createIndexBuffer()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get buffer size
	vk::DeviceSize bufferSize = sizeof(framework->indices[0]) * framework->indices.size();

	if (bufferSize == 0) {
		framework->indices = { 0 };
		bufferSize = sizeof(framework->indices[0]) * framework->indices.size();
	}

	//create buffer
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	//fill buffer
	void* data;
	framework->device.mapMemory(stagingBufferMemory, vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, framework->indices.data(), (size_t)bufferSize);
	framework->device.unmapMemory(stagingBufferMemory);

	vk::Buffer indexBuffer;
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, framework->indexBufferMemory);

	VulkanMemory mem;
	mem.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	framework->device.destroyBuffer(stagingBuffer, nullptr);
	framework->device.freeMemory(stagingBufferMemory, nullptr);

	return indexBuffer;
}

void VulkanBuffer::createBuffer(vk::DeviceSize& size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//define and create buffer
	vk::BufferCreateInfo bufferInfo = { {}, size, usage,vk::SharingMode::eExclusive };


	PSIM_ASSERT(framework->device.createBuffer(&bufferInfo, nullptr, &buffer) == vk::Result::eSuccess, "Failed to create buffer!");

	//allocate buffer memory
	vk::MemoryRequirements memRequirements;
	framework->device.getBufferMemoryRequirements(buffer, &memRequirements);

	VulkanMemory mem;
	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, mem.findMemoryType(memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(framework->device.allocateMemory(&allocInfo, nullptr, &bufferMemory) == vk::Result::eSuccess, "Failed to allocate buffer memory!");

	framework->device.bindBufferMemory(buffer, bufferMemory, 0);
}