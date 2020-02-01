#include "PSIMPCH.h"

#include "Application/Application.h"

Application* CreateApplication()
{
	return new Application();
}

int main() {
	//initialize logging (the three day bug...)
	Log::init();

	/*
	Application app;

	app.run();

	
	*/

	PSIM_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = CreateApplication();
	PSIM_PROFILE_END_SESSION();

	PSIM_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	PSIM_PROFILE_END_SESSION();

	PSIM_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
	delete app;
	PSIM_PROFILE_END_SESSION();

	Log::shutdown();
	system("PAUSE");

	return EXIT_SUCCESS;
}