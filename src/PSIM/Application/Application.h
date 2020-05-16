#pragma once

//vendor files
#include <Platform/Vk/VkRender.h>

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