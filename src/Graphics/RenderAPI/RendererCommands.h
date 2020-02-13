#pragma once
#include "Graphics/RenderAPI/RendererAPI.h"

class RenderCommands
{
public:
	inline static void Init()
	{
		PSIM_PROFILE_FUNCTION();
		s_RendererAPI->Init();
	}
	/*
	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		PSIM_PROFILE_FUNCTION();
		s_RendererAPI->SetViewport(x, y, width, height);
	}
	
	inline static void SetClearColor(const glm::vec4& color)
	{
		PSIM_PROFILE_FUNCTION();
		s_RendererAPI->SetClearColor(color);
	}
	*/

	inline static void Clear()
	{
		PSIM_PROFILE_FUNCTION();
		s_RendererAPI->Clear();
	}
	/*
	inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		PSIM_PROFILE_FUNCTION();
		s_RendererAPI->DrawIndexed(vertexArray);
	}
	*/
private:
	static Scope<RendererAPI> s_RendererAPI;
};

