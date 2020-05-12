#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDebugger
{
public:
	VulkanDebugger();
	~VulkanDebugger();

	vk::DebugUtilsMessengerEXT createNewDebugger();
	vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();
};