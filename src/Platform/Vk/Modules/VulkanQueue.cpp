#include "PSIMPCH.h"
#include "VulkanQueue.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"


VulkanQueue::VulkanQueue()
{
}


VulkanQueue::~VulkanQueue()
{
}

VulkanQueue::QueueFamilyIndices VulkanQueue::findQueueFamilies(vk::PhysicalDevice& device)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//create the queue family indeces
	QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	//check for graphics families in the queue
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		vk::Bool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, framework->surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		//if complete we are done
		if (indices.isComplete()) {
			break;
		}

		//if not go to the next
		i++;
	}

	//if none found, will not return with a value
	return indices;
}