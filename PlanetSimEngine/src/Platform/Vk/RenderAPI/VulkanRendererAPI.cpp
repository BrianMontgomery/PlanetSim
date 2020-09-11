#include "PSIMPCH.h"
#include "VulkanRendererAPI.h"

#include "vulkan/vulkan.hpp"

#include "Vk/FrameWork/VulkanFrameWork.h"

void VulkanRendererAPI::Init()
{
	PSIM_PROFILE_FUNCTION();

	//turn to init after the app has some resources loaded
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


void VulkanRendererAPI::SetViewMatrix(glm::mat4 viewMatrix)
{
	PSIM_PROFILE_FUNCTION();

	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	framework->setViewMatrix(viewMatrix);
}

void VulkanRendererAPI::SetProjectionMatrix(glm::mat4 projectionMatrix)
{
	PSIM_PROFILE_FUNCTION();

	VulkanFrameWork* framework = VulkanFrameWork::getFramework();
	framework->setProjectionMatrix(projectionMatrix);
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


void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	if (vertexArray != framework->getCurrentVertexArray()) {
		framework->setVertexArray(vertexArray);
	}

	for (size_t i = 0; i < framework->getCommandBuffersSize(); i++) {
		framework->commandBufferRecordBegin(&(framework->getCommandBuffers()->at(i)), i);
		framework->commandBufferRecordEnd(&(framework->getCommandBuffers()->at(i)));
	}

	framework->drawFrame();
}