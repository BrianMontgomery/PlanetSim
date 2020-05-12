#pragma once

#include "PSIM/Graphics/Context/GraphicsContext.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

struct GLFWwindow;

VulkanFrameWork *VulkanFrameWork::m_framework = 0;

class VulkanContext : public GraphicsContext
{
public:
	VulkanContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void drawFrame() override;

	virtual void SwapBuffers() override {};


private:
	GLFWwindow* windowHandle;
	VulkanFrameWork* framework;
};