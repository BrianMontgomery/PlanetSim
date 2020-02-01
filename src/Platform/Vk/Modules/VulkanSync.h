#pragma once
#include "vulkan\vulkan.hpp"

class VulkanSync
{
public:
	VulkanSync();
	~VulkanSync();

	void VulkanSync::createSyncObjects(const int& MAX_FRAMES_IN_FLIGHT, size_t& currentFrame, std::vector<vk::Semaphore>& imageAvailableSemaphores, std::vector<vk::Semaphore>& renderFinishedSemaphores, std::vector<vk::Fence>& inFlightFences, std::vector<vk::Fence>& imagesInFlight, std::vector<vk::Image>& swapChainImages, vk::Device& device);
};

