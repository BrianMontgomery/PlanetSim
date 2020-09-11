#pragma once

#include "PSIM/Core/WindowPrototype.h"
#include "PSIM/Graphics/Context/GraphicsContext.h"

#include <GLFW/glfw3.h>

class WindowsWindow : public WindowPrototype
{
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return windowData.Width; }
	inline unsigned int GetHeight() const override { return windowData.Height; }

	// Window attributes
	inline void SetEventCallback(const EventCallbackFn& callback) override { windowData.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	inline virtual void* GetNativeWindow() const { return window; }

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();

private:
	GLFWwindow* window;
	Scope<GraphicsContext> m_Context;

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData windowData;
};