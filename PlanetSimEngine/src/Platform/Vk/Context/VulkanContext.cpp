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

	//TODO: turn to preinit
	framework->init(window);
}