#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

class ValidationLayer
{
public:
	ValidationLayer();
	~ValidationLayer();

	bool checkValidationLayerSupport();

private:
	
};

