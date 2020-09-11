#include "PSIMPCH.h"
#include "Buffer.h"

#include "PSIM/Graphics/Renderer.h"

#include "Platform/Vk/Components/VulkanBuffer.h"

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:    PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanVertexBuffer>(size);
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:    PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanVertexBuffer>(vertices, size);
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:    PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanIndexBuffer>(indices, size);
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}