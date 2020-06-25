//core files
#include "PSIMPCH.h"
#include "Application.h"

#include "Graphics/Renderer.h"
#include "Core/Debug/TimeStep.h"

#include "GLFW/glfw3.h"

//vulkan entry point
//--------------------------------------------------------------------------------------------------------------------------------

Application* Application::s_AppInstance = nullptr;

Application::Application()
{
	PSIM_PROFILE_FUNCTION();

	PSIM_ASSERT(!s_AppInstance, "Application already exists!");
	s_AppInstance = this;
	m_Window = WindowPrototype::Create();
	m_Window->SetEventCallback(PSIM_BIND_EVENT_FN(Application::OnEvent));

	//init the renderer
	Renderer::Init();

#ifdef PSIM_DEBUG
	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
#endif
}

Application::~Application()
{
	PSIM_PROFILE_FUNCTION();

	Renderer::Shutdown();
}

void Application::PushLayer(Layers* layer)
{
	PSIM_PROFILE_FUNCTION();

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layers* layer)
{
	PSIM_PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::OnEvent(Event& e)
{
	PSIM_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(PSIM_BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(PSIM_BIND_EVENT_FN(Application::OnWindowResize));

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
	{
		(*it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Application::Run()
{
	PSIM_PROFILE_FUNCTION();

	while (m_Running)
	{
		PSIM_PROFILE_SCOPE("RunLoop");

		float time = (float)glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;
		std::cout << m_LastFrameTime << std::endl;

		if (!m_Minimized)
		{

			{
				PSIM_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layers* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

#ifdef PSIM_DEBUG
			m_ImGuiLayer->Begin();
			{
				PSIM_PROFILE_SCOPE("LayerStack OnImGuiRender");
				for (Layers* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
#endif
			
		}

		m_Window->OnUpdate();		//has drawFrame()
	}
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	PSIM_PROFILE_FUNCTION();

	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}