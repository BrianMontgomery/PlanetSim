#include "PSIMPCH.h"
#include "Renderer.h"


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

	//Renderer2D::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	PSIM_PROFILE_FUNCTION();
	//RenderCommand::SetViewport(0, 0, width, height);
}

/*
void Renderer::BeginScene(OrthographicCamera& camera)
{
	PSIM_PROFILE_FUNCTION();
	s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	PSIM_PROFILE_FUNCTION();
	shader->Bind();
	shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	shader->SetMat4("u_Transform", transform);

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
*/