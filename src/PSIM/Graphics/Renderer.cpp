#include "PSIMPCH.h"
#include "Renderer.h"

#include "PSIM/Graphics/Renderer2D.h"
#include "PSIM/Graphics/RenderAPI/RendererCommands.h"


Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

void Renderer::Init()
{
	PSIM_PROFILE_FUNCTION();

	RenderCommands::Init();
	//Renderer2D::Init();
}

void Renderer::Shutdown()
{
	PSIM_PROFILE_FUNCTION();

	//Renderer2D::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	PSIM_PROFILE_FUNCTION();
	RenderCommands::SetViewport(0, 0, width, height);
}


void Renderer::BeginScene(OrthographicCamera& camera)
{
	PSIM_PROFILE_FUNCTION();
	s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<LinkedShader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	PSIM_PROFILE_FUNCTION();
	shader->bind();
	shader->setMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	shader->setMat4("u_Transform", transform);

	vertexArray->Bind();
	RenderCommands::DrawIndexed(vertexArray);
}
