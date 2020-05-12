#include "PSIMPCH.h"
#include "PSIM/Graphics/Shader.h"

#include "PSIM/Graphics/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vk/Modules/VulkanShader.h"

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shaders Library
//---------------------------------------------------------------------------------------------------------------------------------------

Ref<ShaderLibrary> ShaderLibrary::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  return CreateRef<OpenGLLinkedShaderLibrary>();
	case RendererAPI::API::Vulkan:  return CreateRef<VulkanLinkedShaderLibrary>();
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------