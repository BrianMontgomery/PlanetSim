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

	virtual void SwapBuffers() override {};


private:
	GLFWwindow* window;
	VulkanFrameWork* framework;
};