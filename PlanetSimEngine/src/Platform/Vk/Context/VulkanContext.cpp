#include "PSIMPCH.h"
#include "VulkanContext.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"

VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	: window(windowHandle)
{
	PSIM_ASSERT(windowHandle, "Window handle is null!");
	this->framework = framework->getFramework();
}

void VulkanContext::Init()
{
	PSIM_PROFILE_FUNCTION();
	framework = VulkanFrameWork::getFramework();
	//library initialization (glad loader in OpenGL)
	framework->init(window);

	//version checking done at device level
}

void VulkanContext::drawFrame()
{
	PSIM_PROFILE_FUNCTION();

	for (size_t i = 0; i < framework->commandBuffers.size(); i++) {
		framework->commandBufferRecordBegin(i);
		framework->commandBufferRecordEnd(i);
	}
	framework->drawFrame();
}