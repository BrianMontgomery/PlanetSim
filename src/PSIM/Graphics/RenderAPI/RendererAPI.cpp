#include "PSIMPCH.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vk/RenderAPI/VulkanRendererAPI.h"

Scope<RendererAPI> RendererAPI::Create()
{
	PSIM_PROFILE_FUNCTION();
	switch (s_API)
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
	case RendererAPI::API::Vulkan:  return CreateScope<VulkanRendererAPI>();
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}