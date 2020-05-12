#pragma once
#include "vulkan/vulkan.hpp"
#include <optional>

class VulkanQueue
{
public:
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return (graphicsFamily.has_value() && presentFamily.has_value());
		}
	};

	VulkanQueue();
	~VulkanQueue();

	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice& device);
};

