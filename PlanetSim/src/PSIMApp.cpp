#include <PlanetSimEngine.h>
#include <PSIM/Core/EntryPoint.h>

#include "PSIMAppLayer/PSIMAppLayer.h"
//#include "ExampleLayer.h"

class PSIMApp : public Application
{
public:
	PSIMApp()
	{
		PushLayer(new PSIMAppLayer());
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
