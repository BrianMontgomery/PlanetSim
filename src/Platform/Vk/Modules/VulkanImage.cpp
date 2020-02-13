#include "PSIMPCH.h"
#include "VulkanImage.h"

#include "Platform/Vk/Modules/VulkanMemory.h"


VulkanImage::VulkanImage()
{
}


VulkanImage::~VulkanImage()
{
}

vk::ImageView VulkanImage::createImageView(vk::Image& image, vk::Device& device, vk::Format format, vk::ImageAspectFlags aspectFlags) 
{
	PSIM_PROFILE_FUNCTION();
	//define image view
	vk::ImageViewCreateInfo viewInfo = { {}, image, vk::ImageViewType::e2D, format, {}, {aspectFlags, 0, 1, 0, 1} };

	//create it
	vk::ImageView imageView;
	PSIM_ASSERT(device.createImageView(&viewInfo, nullptr, &imageView) == vk::Result::eSuccess, "Failed to create texture image view!");

	return imageView;
}

vk::Image VulkanImage::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
{
	PSIM_PROFILE_FUNCTION();
	//define an image 
	vk::ImageCreateInfo imageInfo = { {}, vk::ImageType::e2D, format, vk::Extent3D { width, height, 1 }, 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive };

	//create image
	PSIM_ASSERT(device.createImage(&imageInfo, nullptr, &image) == vk::Result::eSuccess, "Failed to create image!");

	//allocate memory
	vk::MemoryRequirements memRequirements;
	device.getImageMemoryRequirements(image, &memRequirements);

	VulkanMemory mem;
	vk::MemoryAllocateInfo allocInfo = { memRequirements.size, mem.findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties) };

	PSIM_ASSERT(device.allocateMemory(&allocInfo, nullptr, &imageMemory) == vk::Result::eSuccess, "Failed to allocate image memory!");

	//bind image to an image memory object
	device.bindImageMemory(image, imageMemory, 0);

	return image;
}