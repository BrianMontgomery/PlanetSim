#pragma once
#include <map>

#include "PSIM/Graphics/Components/Buffer.h"
#include "vulkan/vulkan.hpp"

class VulkanBuffer
{
public:
	VulkanBuffer();
	~VulkanBuffer();

	uint32_t createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	void destroyBuffer();

	vk::Buffer getBuffer() { return buffer; }
	vk::DeviceMemory getBufferMemory() { return bufferMemory; }

	uint32_t getID() { return m_RendererID; }

	static Ref<VulkanBuffer> Create();

private:
	uint32_t m_RendererID;
	vk::Buffer buffer;
	vk::DeviceMemory bufferMemory;
};

class VulkanBufferList
{
public:
	VulkanBufferList();
	~VulkanBufferList();

	static void init();
	static void add(Ref<VulkanBuffer> buffer);
	static Ref<VulkanBuffer> get(uint32_t ID);
	static void remove(uint32_t ID);
	static Ref<VulkanBuffer> getBaseBuffer();
private:
	static std::map<uint32_t, Ref<VulkanBuffer>> bufferList;
	static uint32_t baseBufferID;
};

class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(uint32_t size);
	VulkanVertexBuffer(float* vertices, uint32_t size);
	virtual ~VulkanVertexBuffer();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetData(const void* data, uint32_t size) override;

	virtual const BufferLayout& GetLayout() const override { return m_Layout; }
	virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	virtual uint32_t getID() const override { return m_BufferID; }
private:
	uint32_t m_BufferID;
	BufferLayout m_Layout;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
	VulkanIndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~VulkanIndexBuffer();

	virtual void Bind() const;
	virtual void Unbind() const;

	virtual uint32_t GetCount() const { return m_Count; }

	virtual uint32_t getID() const override { return m_BufferID; }
private:
	uint32_t m_Count;
	uint32_t m_BufferID;
};