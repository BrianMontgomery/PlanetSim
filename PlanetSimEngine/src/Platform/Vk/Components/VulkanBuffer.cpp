#include "PSIMPCH.h"
#include "VulkanBuffer.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

std::map<uint32_t, Ref<VulkanBuffer>> VulkanBufferList::bufferList = {};
uint32_t VulkanBufferList::baseBufferID = 0;

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride() * size);

	Ref<VulkanBuffer> vertexBuffer = VulkanBuffer::Create(); 
	m_BufferID = vertexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	
	
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride() * size);

	Ref<VulkanBuffer> vertexBuffer = VulkanBuffer::Create();
	m_BufferID = vertexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	
	SetData(vertices, size);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	PSIM_PROFILE_FUNCTION();

	VulkanBufferList::remove(m_BufferID);
}

void VulkanVertexBuffer::Bind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VulkanVertexBuffer::Unbind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VulkanVertexBuffer::SetData(const void* vertices, uint32_t size)
{
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride() * size);

	//create buffer
	VulkanBufferList::getBaseBuffer()->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data;
	framework->getDevice().mapMemory(VulkanBufferList::getBaseBuffer()->getBufferMemory(), vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, vertices, (size_t)bufferSize);
	framework->getDevice().unmapMemory(VulkanBufferList::getBaseBuffer()->getBufferMemory());

	std::cout << VulkanBufferList::get(m_BufferID);
	VulkanBufferList::get(m_BufferID)->copyBuffer(VulkanBufferList::getBaseBuffer()->getBuffer(), VulkanBufferList::get(m_BufferID)->getBuffer(), bufferSize);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//get buffer size
	vk::DeviceSize bufferSize = sizeof(indices[0]) * count;

	//create buffer
	VulkanBufferList::getBaseBuffer()->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	//fill buffer
	void* data;
	framework->getDevice().mapMemory(VulkanBufferList::getBaseBuffer()->getBufferMemory(), vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, indices, (size_t)bufferSize);
	framework->getDevice().unmapMemory(VulkanBufferList::getBaseBuffer()->getBufferMemory());

	Ref<VulkanBuffer> indexBuffer = VulkanBuffer::Create();
	m_BufferID = indexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

	indexBuffer->copyBuffer(VulkanBufferList::getBaseBuffer()->getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	PSIM_PROFILE_FUNCTION();

	VulkanBufferList::remove(m_BufferID);
}

void VulkanIndexBuffer::Bind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void VulkanIndexBuffer::Unbind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/////////////////////////////////////////////////////////////////////////////
// BufferBase ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

Ref<VulkanBuffer> VulkanBuffer::VulkanBuffer::Create()
{
	Ref<VulkanBuffer> buffer = CreateRef<VulkanBuffer>();
	VulkanBufferList::add(buffer);
	return buffer;

	PSIM_ASSERT(false, "Can't create VulkanBuffer");
	return nullptr;
}

VulkanBuffer::VulkanBuffer()
{
	RendererID ID;
	m_RendererID = ID.getID();
}

VulkanBuffer::~VulkanBuffer()
{
	destroyBuffer();
}

uint32_t VulkanBuffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//define and create buffer
	vk::BufferCreateInfo bufferInfo = { {}, size, usage,vk::SharingMode::eExclusive };

	PSIM_ASSERT(framework->getDevice().createBuffer(&bufferInfo, nullptr, &buffer) == vk::Result::eSuccess, "Failed to create buffer!");

	//allocate buffer memory
	vk::MemoryRequirements memRequirements;
	framework->getDevice().getBufferMemoryRequirements(buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, framework->findMemoryType(memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(framework->getDevice().allocateMemory(&allocInfo, nullptr, &bufferMemory) == vk::Result::eSuccess, "Failed to allocate buffer memory!");

	framework->getDevice().bindBufferMemory(buffer, bufferMemory, 0);

	return m_RendererID;
}

void VulkanBuffer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//copy one buffer to another
	vk::CommandBuffer commandBuffer = framework->beginSingleTimeCommands();

	vk::BufferCopy copyRegion = { 0, 0, size };
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	framework->endSingleTimeCommands(commandBuffer);
}

void VulkanBuffer::destroyBuffer()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	framework->getDevice().destroyBuffer(buffer, nullptr);
	framework->getDevice().freeMemory(bufferMemory, nullptr);
}

/////////////////////////////////////////////////////////////////////////////
// BufferList ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VulkanBufferList::VulkanBufferList()
{
}

VulkanBufferList::~VulkanBufferList()
{
}

void VulkanBufferList::init()
{
	Ref<VulkanBuffer> buffer = VulkanBuffer::Create();
	baseBufferID = buffer->createBuffer(1, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	add(buffer);
}

void VulkanBufferList::add(Ref<VulkanBuffer> buffer)
{
	std::map<uint32_t, Ref<VulkanBuffer>>::iterator it;
	it = bufferList.find(buffer->getID());
	
	if (it == bufferList.end())
	{
		bufferList[buffer->getID()] = buffer;
	}
	else {
		PSIM_WARN("Buffer with ID {0} already exists!", buffer->getID());
	}
}

Ref<VulkanBuffer> VulkanBufferList::get(uint32_t ID)
{
	std::map<uint32_t, Ref<VulkanBuffer>>::iterator it;
	it = bufferList.find(ID);

	if (it != bufferList.end())
	{
		return bufferList[ID];
	}
	else {
		PSIM_WARN("Buffer with ID {0} doesn't exists!", ID);
	}
}

void VulkanBufferList::remove(uint32_t ID)
{
	std::map<uint32_t, Ref<VulkanBuffer>>::iterator it;
	it = bufferList.find(ID);

	if (it != bufferList.end())
	{
		bufferList.erase(it);
	}
	else {
		PSIM_WARN("Buffer with ID {0} doesn't exists!", ID);
	}
}

Ref<VulkanBuffer> VulkanBufferList::getBaseBuffer()
{
	return (get(baseBufferID));
}