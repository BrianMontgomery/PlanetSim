#include <PlanetSimEngine.h>
#include <PSIM/Core/EntryPoint.h>

//#include "Sandbox2D.h"
//#include "ExampleLayer.h"

class PSIMApp : public Application
{
public:
	PSIMApp()
	{
		// PushLayer(new ExampleLayer());
		//PushLayer(new Sandbox2D());
	}

	~PSIMApp()
	{
	}
};

Application* CreateApplication()
{
	return new PSIMApp();
}
