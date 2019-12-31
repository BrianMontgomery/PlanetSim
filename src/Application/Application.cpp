//core files
#include "PSIMPCH.h"
#include "Core.h"

#include "Application.h"
#include "Logging/Log.h"

//vulkan entry point
//--------------------------------------------------------------------------------------------------------------------------------
void Application::run()
{
	//loop
	mainLoop();
}

void Application::mainLoop()
{
	VkRender vkRender;

	bool close = false;
	while (!close) {								
		close = vkRender.mainLoop();				
	}

	PSIM_CORE_WARN("Shutdown Initiated");
}