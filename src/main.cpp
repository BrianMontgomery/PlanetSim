#include "PSIMPCH.h"

#include "Application/Application.h"

Application* CreateApplication()
{
	return new Application();
}

int main() 
{
	PSIM_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	//initialize logging (the three day bug...)
	Log::init();

	//create app
	auto app = CreateApplication();
	PSIM_PROFILE_END_SESSION();

	PSIM_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	//run app
	app->Run();
	PSIM_PROFILE_END_SESSION();

	PSIM_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
	//delete app
	delete app;

	//shutdown log
	Log::shutdown();
	PSIM_PROFILE_END_SESSION();

	system("PAUSE");

	return EXIT_SUCCESS;
}