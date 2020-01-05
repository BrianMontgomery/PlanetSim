#include "PSIMPCH.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>
#include "vulkan/vulkan.hpp"

VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	PSIM_ASSERT(windowHandle, "Window handle is null!")
}

void VulkanContext::Init()
{
	PSIM_PROFILE_FUNCTION();

	glfwMakeContextCurrent(m_WindowHandle);
}

void VulkanContext::SwapBuffers()
{
	PSIM_PROFILE_FUNCTION();

	glfwSwapBuffers(m_WindowHandle);
}