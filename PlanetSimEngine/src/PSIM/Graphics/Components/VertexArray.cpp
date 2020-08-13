#include "PSIMPCH.h"
#include "VertexArray.h"

#include "PSIM/Graphics/Renderer.h"
#include "Platform/Vk/Components/VulkanVertexArray.h"

Ref<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanVertexArray>();
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}