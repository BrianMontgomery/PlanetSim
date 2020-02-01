#include "PSIMPCH.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"

VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	: windowHandle(windowHandle)
{
	PSIM_ASSERT(windowHandle, "Window handle is null!")
}

void VulkanContext::Init()
{
	PSIM_PROFILE_FUNCTION();
	framework.init(windowHandle);
}

void VulkanContext::drawFrame()
{
	PSIM_PROFILE_FUNCTION();

	framework.drawFrame(windowHandle);
}