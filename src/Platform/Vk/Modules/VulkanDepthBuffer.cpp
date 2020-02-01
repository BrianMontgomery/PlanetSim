#include "PSIMPCH.h"
#include "VulkanDepthBuffer.h"

#include "Platform/Vk/Modules/VulkanImage.h"

VulkanDepthBuffer::VulkanDepthBuffer()
{
}


VulkanDepthBuffer::~VulkanDepthBuffer()
{
}

vk::ImageView VulkanDepthBuffer::createDepthResources(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::Extent2D& swapchainExtent, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory)
{
	//get depth format
	vk::Format depthFormat = findDepthFormat(physicalDevice);

	//create depth resources
	VulkanImage imageMaker;
	imageMaker.createImage(swapchainExtent.width, swapchainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
		physicalDevice, device, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
	vk::ImageView depthImageView = imageMaker.createImageView(depthImage, device, depthFormat, vk::ImageAspectFlagBits::eDepth);

	return depthImageView;
}

vk::Format VulkanDepthBuffer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, vk::PhysicalDevice& physicalDevice)
{
	//check for optimal formats
	for (vk::Format format : candidates) {
		vk::FormatProperties props;
		physicalDevice.getFormatProperties(format, &props);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	PSIM_CORE_ERROR("failed to find supported format!");

	return vk::Format::eUndefined;
}

vk::Format VulkanDepthBuffer::findDepthFormat(vk::PhysicalDevice& physicalDevice)
{
	//return the best format here
	return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment, physicalDevice);
}

bool VulkanDepthBuffer::hasStencilComponent(vk::Format format)
{
	//check for this stencil component
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}