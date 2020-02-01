#pragma once

#include "Graphics/Context/GraphicsContext.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

struct GLFWwindow;

class VulkanContext : public GraphicsContext
{
public:
	VulkanContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void drawFrame() override;

private:
	GLFWwindow* windowHandle;
	VulkanFrameWork framework;
};