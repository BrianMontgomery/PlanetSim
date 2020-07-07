#include "PSIMPCH.h"
#include "VulkanBuffer.h"

/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
{
	PSIM_PROFILE_FUNCTION();

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	PSIM_PROFILE_FUNCTION();

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	PSIM_PROFILE_FUNCTION();

	glDeleteBuffers(1, &m_RendererID);
}

void VulkanVertexBuffer::Bind() const
{
	PSIM_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VulkanVertexBuffer::Unbind() const
{
	PSIM_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count)
{
	PSIM_PROFILE_FUNCTION();

	glCreateBuffers(1, &m_RendererID);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	PSIM_PROFILE_FUNCTION();

	glDeleteBuffers(1, &m_RendererID);
}

void VulkanIndexBuffer::Bind() const
{
	PSIM_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void VulkanIndexBuffer::Unbind() const
{
	PSIM_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}