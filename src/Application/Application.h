#pragma once

#include "Events/Events.h"
#include "Events/ApplicationEvents.h"
#include "Core/WindowPrototype.h"

/*
class Application
{
public:
	//public func
	void run();

	//public var for vulkan
	bool framebufferResized = false;

private:
	//private structs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------

	//member funcs
	//--------------------------------------------------------------------------------------------------------------------------------
	void mainLoop();
};
*/

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	void OnEvent(Event& e);

	//void PushLayer(Layer* layer);
	//void PushOverlay(Layer* layer);

	inline WindowPrototype& GetWindow() { return *m_Window; }

	inline static Application& Get() { return *s_AppInstance; }
private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
private:
	std::unique_ptr<WindowPrototype> m_Window;
	//ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	bool m_Minimized = false;
	//LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;
private:
	static Application* s_AppInstance;
};

// To be defined in CLIENT
Application* CreateApplication();
