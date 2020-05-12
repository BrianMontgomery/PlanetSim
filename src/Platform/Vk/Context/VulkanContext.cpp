#include "PSIMPCH.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"

VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	: windowHandle(windowHandle)
{
	PSIM_ASSERT(windowHandle, "Window handle is null!");
	this->framework = framework->getFramework();
}

void VulkanContext::Init()
{
	PSIM_PROFILE_FUNCTION();

	//library initialization (glad loader in OpenGL)
	framework->init(windowHandle);

	//version checking done at device level
}

void VulkanContext::drawFrame()
{
	PSIM_PROFILE_FUNCTION();

	framework->drawFrame(windowHandle);
}