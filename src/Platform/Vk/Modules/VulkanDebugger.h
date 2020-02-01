#pragma once
#include "vulkan/vulkan.hpp"

class VulkanDebugger
{
public:
	VulkanDebugger();
	~VulkanDebugger();

	vk::DebugUtilsMessengerEXT createNewDebugger(vk::Instance& instance);
	vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();
};

