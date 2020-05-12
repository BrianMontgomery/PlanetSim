#pragma once
#include "vulkan\vulkan.hpp"

class VulkanSync
{
public:
	VulkanSync();
	~VulkanSync();

	void VulkanSync::createSyncObjects(const int& MAX_FRAMES_IN_FLIGHT, size_t& currentFrame);
};

