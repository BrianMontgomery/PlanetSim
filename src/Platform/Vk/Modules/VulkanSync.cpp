#include "PSIMPCH.h"
#include "VulkanSync.h"


VulkanSync::VulkanSync()
{
}


VulkanSync::~VulkanSync()
{
}

void VulkanSync::createSyncObjects(const int& MAX_FRAMES_IN_FLIGHT, size_t& currentFrame, std::vector<vk::Semaphore>& imageAvailableSemaphores, std::vector<vk::Semaphore>& renderFinishedSemaphores, std::vector<vk::Fence>& inFlightFences, std::vector<vk::Fence>& imagesInFlight, std::vector<vk::Image>& swapChainImages, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();

	//get sync objects for current frames and resize
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size());

	//reset sync objects info
	vk::SemaphoreCreateInfo semaphoreInfo = {};

	vk::FenceCreateInfo fenceInfo = { vk::FenceCreateFlagBits::eSignaled };

	//recreate sync objects
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		PSIM_ASSERT(device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) == vk::Result::eSuccess &&
			device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) == vk::Result::eSuccess &&
			device.createFence(&fenceInfo, nullptr, &inFlightFences[i]) == vk::Result::eSuccess, "Failed to create synchronization objects for a frame!");
	}

	PSIM_CORE_INFO("Sync Objects Created");
}