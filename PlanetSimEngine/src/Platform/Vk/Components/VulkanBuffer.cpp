#include "PSIMPCH.h"
#include "VulkanBuffer.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride()) * size;

	Ref<VulkanBuffer> vertexBuffer = VulkanBuffer::Create(); 
	m_BufferID = vertexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	
	SetData(nullptr, size);
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//get vertex buffer size
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride()) * size;

	Ref<VulkanBuffer> vertexBuffer = VulkanBuffer::Create();
	m_BufferID = vertexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	
	SetData(vertices, size);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	framework->getBufferList()->remove(m_BufferID);
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
	vk::DeviceSize bufferSize = sizeof(m_Layout.GetStride()) * size;

	//create buffer
	framework->getBufferList()->getBaseBuffer()->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data;
	framework->getDevice().mapMemory(framework->getBufferList()->getBaseBuffer()->getBufferMemory(), vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, vertices, (size_t)bufferSize);
	framework->getDevice().unmapMemory(framework->getBufferList()->getBaseBuffer()->getBufferMemory());

	std::cout << framework->getBufferList()->get(m_BufferID);
	framework->getBufferList()->get(m_BufferID)->copyBuffer(framework->getBufferList()->getBaseBuffer()->getBuffer(), framework->getBufferList()->get(m_BufferID)->getBuffer(), bufferSize);

	framework->getBufferList()->getBaseBuffer()->destroyBuffer();
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
	framework->getBufferList()->getBaseBuffer()->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	//fill buffer
	void* data;
	framework->getDevice().mapMemory(framework->getBufferList()->getBaseBuffer()->getBufferMemory(), vk::DeviceSize(), bufferSize, vk::MemoryMapFlags(), &data);
	memcpy(data, indices, (size_t)bufferSize);
	framework->getDevice().unmapMemory(framework->getBufferList()->getBaseBuffer()->getBufferMemory());

	Ref<VulkanBuffer> indexBuffer = VulkanBuffer::Create();
	m_BufferID = indexBuffer->createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

	indexBuffer->copyBuffer(framework->getBufferList()->getBaseBuffer()->getBuffer(), indexBuffer->getBuffer(), bufferSize);

	framework->getBufferList()->getBaseBuffer()->destroyBuffer();
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	framework->getBufferList()->remove(m_BufferID);
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
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	Ref<VulkanBuffer> buffer = CreateRef<VulkanBuffer>();
	framework->getBufferList()->add(buffer);
	return buffer;

	PSIM_ASSERT(false, "Can't create VulkanBuffer");
	return nullptr;
}

VulkanBuffer::VulkanBuffer()
{
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	m_ID = framework->getBufferList()->getNextID();
}

VulkanBuffer::~VulkanBuffer()
{
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

	return m_ID;
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
	bufferList = {};
	baseBufferID = 0;
}

VulkanBufferList::~VulkanBufferList()
{
}

void VulkanBufferList::init()
{
	Ref<VulkanBuffer> buffer = VulkanBuffer::Create();
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
		PSIM_WARN("Buffer with ID {0} doesn't exist!", ID);
	}
}

uint32_t VulkanBufferList::getNextID()
{
	return bufferList.size();
}

void VulkanBufferList::remove(uint32_t ID)
{
	if (!bufferList.empty()) {
		std::map<uint32_t, Ref<VulkanBuffer>>::iterator it;
		it = bufferList.find(ID);

		if (it != bufferList.end())
		{
			bufferList[ID]->destroyBuffer();
			bufferList.erase(it);
		}
		else {
			PSIM_WARN("Buffer with ID {0} doesn't exist!", ID);
		}
	}
	else {
		PSIM_WARN("Buffer with ID {0} doesn't exist!", ID);
	}
}

Ref<VulkanBuffer> VulkanBufferList::getBaseBuffer()
{
	return (get(baseBufferID));
}

void VulkanBufferList::cleanup()
{
	if (!bufferList.empty()) {
		for (int i = 0; i < bufferList.size(); i++) {
			bufferList[i]->destroyBuffer();
		}
		bufferList.clear();
	}
}