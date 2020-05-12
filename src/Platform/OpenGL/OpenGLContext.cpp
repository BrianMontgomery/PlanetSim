#include "PSIMPCH.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	PSIM_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGLContext::Init()
{
	PSIM_PROFILE_FUNCTION();

	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	PSIM_ASSERT(status, "Failed to initialize Glad!");

	PSIM_CORE_INFO("OpenGL Info:");
	PSIM_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	PSIM_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	PSIM_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

#ifdef PSIM_ENABLE_ASSERTS
	int versionMajor;
	int versionMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

	PSIM_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 4), "Hazel requires at least OpenGL version 4.4!");
	std::cout << std::endl;
	PSIM_CORE_INFO("Initializing OpenGL");
#endif
}

void OpenGLContext::drawFrame()
{

}

void OpenGLContext::SwapBuffers()
{
	PSIM_PROFILE_FUNCTION();

	glfwSwapBuffers(m_WindowHandle);
}