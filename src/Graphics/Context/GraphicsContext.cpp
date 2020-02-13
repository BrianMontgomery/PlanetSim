#include "PSIMPCH.h"
#include "GraphicsContext.h"

#include "Graphics/Renderer.h"
#include "Platform/Vk/Context/VulkanContext.h"

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
	PSIM_PROFILE_FUNCTION();
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  PSIM_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}