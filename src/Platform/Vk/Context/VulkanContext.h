#pragma once

#include "Graphics/Context/GraphicsContext.h"

struct GLFWwindow;

class VulkanContext : public GraphicsContext
{
public:
	VulkanContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void SwapBuffers() override;
private:
	GLFWwindow* m_WindowHandle;
};