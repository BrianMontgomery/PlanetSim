#include "PSIMPCH.h"
#include "VulkanSync.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

VulkanSync::VulkanSync()
{
}


VulkanSync::~VulkanSync()
{
}

void VulkanSync::createSyncObjects(const int& MAX_FRAMES_IN_FLIGHT, size_t& currentFrame)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//get sync objects for current frames and resize
	framework->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	framework->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	framework->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	framework->imagesInFlight.resize(framework->swapchainImages.size());

	//reset sync objects info
	vk::SemaphoreCreateInfo semaphoreInfo = {};

	vk::FenceCreateInfo fenceInfo = { vk::FenceCreateFlagBits::eSignaled };

	//recreate sync objects
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		PSIM_ASSERT(framework->device.createSemaphore(&semaphoreInfo, nullptr, &framework->imageAvailableSemaphores[i]) == vk::Result::eSuccess &&
			framework->device.createSemaphore(&semaphoreInfo, nullptr, &framework->renderFinishedSemaphores[i]) == vk::Result::eSuccess &&
			framework->device.createFence(&fenceInfo, nullptr, &framework->inFlightFences[i]) == vk::Result::eSuccess, "Failed to create synchronization objects for a frame!");
	}

	PSIM_CORE_INFO("Sync Objects Created");
}