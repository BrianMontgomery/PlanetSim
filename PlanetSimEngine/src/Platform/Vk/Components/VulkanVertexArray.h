#pragma once

#include "PSIM/Graphics/Components/VertexArray.h"
#include "vulkan/vulkan.hpp"

class VulkanVertexArray : public VertexArray
{
public:
	VulkanVertexArray();

	virtual ~VulkanVertexArray();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
	virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffer; }
	virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

	virtual void* getBindingDescription() override;
	virtual void* getAttributeDescriptions() override;

	virtual void* getVertexBuffersBuffers() override;
	virtual void* getIndexBufferBuffer() override;

	virtual void cleanUp() override;

	std::vector<vk::Buffer> vBuffBuffers;
	vk::VertexInputBindingDescription bindingDescription;
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

private:
	uint32_t m_VertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> m_VertexBuffer;
	int vertexElementCount = 0;
	Ref<IndexBuffer> m_IndexBuffer;
};