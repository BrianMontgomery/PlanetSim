#include "PSIMPCH.h"
#include "VulkanRendererAPI.h"

#include "vulkan/vulkan.hpp"

#include "Vk/FrameWork/VulkanFrameWork.h"

void VulkanRendererAPI::Init()
{
	PSIM_PROFILE_FUNCTION();

#ifdef PSIM_DEBUG
	//glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback(OpenGLMessageCallback, nullptr);

	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_DEPTH_TEST);
}


void VulkanRendererAPI::SetViewProjectionMatrix(glm::mat4 viewProjectionMatrix)
{
	PSIM_PROFILE_FUNCTION();

	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	framework->setVPMatrix(viewProjectionMatrix);
}

void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
{
	PSIM_PROFILE_FUNCTION();

	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	framework->setClearColor(color);
}


void VulkanRendererAPI::Clear()
{
	PSIM_PROFILE_FUNCTION();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


/*void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
{
	PSIM_PROFILE_FUNCTION();

	//glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	//glBindTexture(GL_TEXTURE_2D, 0);
}*/