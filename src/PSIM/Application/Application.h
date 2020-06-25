#pragma once

#include "Core/Events/Events.h"
#include "Core/Events/ApplicationEvents.h"
#include "Core/WindowPrototype.h"
#include "Core/Layers/LayerStack.h"
#ifdef PSIM_DEBUG
#include "imgui/ImGuiLayer.h"
#endif

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	void OnEvent(Event& e);

	void PushLayer(Layers* layer);
	void PushOverlay(Layers* layer);

	inline WindowPrototype& GetWindow() { return *m_Window; }

	inline static Application& Get() { return *s_AppInstance; }

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	std::unique_ptr<WindowPrototype> m_Window;
#ifdef PSIM_DEBUG
	ImGuiLayer* m_ImGuiLayer;
#endif
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;

private:
	static Application* s_AppInstance;
};

// To be defined in CLIENT
Application* CreateApplication();