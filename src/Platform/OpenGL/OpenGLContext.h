#pragma once
#include "PSIMPCH.h"
#include "PSIM/Core.h"
#include "PSIM/Graphics/Context/GraphicsContext.h"

struct GLFWwindow;
 
class OpenGLContext : public GraphicsContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void drawFrame() override;
	virtual void SwapBuffers() override;
private:
	GLFWwindow* m_WindowHandle;
};