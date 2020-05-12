#include "PSIMPCH.h"
#include "VulkanDepthBuffer.h"

#include "Platform/Vk/Modules/VulkanImage.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

VulkanDepthBuffer::VulkanDepthBuffer()
{
}


VulkanDepthBuffer::~VulkanDepthBuffer()
{
}

vk::ImageView VulkanDepthBuffer::createDepthResources()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//get depth format
	vk::Format depthFormat = findDepthFormat();

	//create depth resources
	VulkanImage imageMaker;
	imageMaker.createImage(framework->swapchainExtent.width, framework->swapchainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
	 vk::MemoryPropertyFlagBits::eDeviceLocal, framework->depthImage, framework->depthImageMemory);
	vk::ImageView depthImageView = imageMaker.createImageView(framework->depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);

	return depthImageView;
}

vk::Format VulkanDepthBuffer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//check for optimal formats
	for (vk::Format format : candidates) {
		vk::FormatProperties props;
		framework->physicalDevice.getFormatProperties(format, &props);

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

vk::Format VulkanDepthBuffer::findDepthFormat()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//return the best format here
	return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool VulkanDepthBuffer::hasStencilComponent(vk::Format format)
{
	PSIM_PROFILE_FUNCTION();
	//check for this stencil component
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}