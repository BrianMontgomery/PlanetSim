#include "PSIMPCH.h"
#include "PSIM/Core.h"
#include "PSIM/Graphics/Buffer.h"

#include "PSIM/Graphics/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
		case RendererAPI::API::Vulkan:  PSIM_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, size);
		case RendererAPI::API::Vulkan:  PSIM_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}