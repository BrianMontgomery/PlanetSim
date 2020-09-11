#include "PSIMPCH.h"
#include "Graphics/Renderer.h"

#include "PSIM/Graphics/RenderAPI/RendererCommands.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"


Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

void Renderer::Init()
{
	PSIM_PROFILE_FUNCTION();

	RenderCommands::Init();
//	Renderer2D::Init();
}

void Renderer::Shutdown()
{
	PSIM_PROFILE_FUNCTION();

//	Renderer2D::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	//RenderCommand::SetViewport(0, 0, width, height);
}


void Renderer::BeginScene(Camera& camera)
{
	s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::EndScene()
{
}


void Renderer::Submit(const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	//shader->Bind();
	//shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	//shader->SetMat4("u_Transform", transform);

	RenderCommands::DrawIndexed(vertexArray);
}
