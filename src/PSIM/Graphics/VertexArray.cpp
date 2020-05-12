#include "PSIMPCH.h"
#include "PSIM/Graphics/VertexArray.h"

#include "PSIM/Graphics/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

Ref<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		case RendererAPI::API::Vulkan:  PSIM_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}