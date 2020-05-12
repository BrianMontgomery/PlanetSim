#pragma once

#include "PSIM/Events/Events.h"
#include "PSIM/Events/ApplicationEvents.h"
#include "PSIM/WindowPrototype.h"
#include "PSIM/Layers/LayerStack.h"

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	void OnEvent(Event& e);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

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
	LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;
private:
	static Application* s_AppInstance;
};

// To be defined in CLIENT
Application* CreateApplication();
