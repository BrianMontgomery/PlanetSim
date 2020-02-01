#include "PSIMPCH.h"
#include "VulkanTexture.h"

#include "stb/stb_image.h"
#include "Platform/Vk/Modules/VulkanBuffer.h"
#include "Platform/Vk/Modules/VulkanImage.h"
#include "Platform/Vk/Modules/VulkanMemory.h"


VulkanTexture::VulkanTexture()
{
}


VulkanTexture::~VulkanTexture()
{
}

vk::Image VulkanTexture::createTextureImage(const char* TEXTURE_PATH, vk::CommandPool& commandPool, vk::Device& device, vk::Queue& graphicsQueue, vk::PhysicalDevice& physicalDevice, vk::DeviceMemory& textureImageMemory)
{
	//load texture
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(TEXTURE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	PSIM_ASSERT(pixels, "Failed to load texture image!");

	//designate a buffer for the texture
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	VulkanBuffer buff;
	buff.createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory, physicalDevice, device);

	//transfer texture data to new buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	//unload image
	stbi_image_free(pixels);

	//create image
	vk::Image textureImage;
	VulkanImage imageMaker;
	imageMaker.createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, physicalDevice, device, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

	//transition data format to optimal
	VulkanMemory mem;
	mem.transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, commandPool, device, graphicsQueue);
	mem.copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool, device, graphicsQueue);
	mem.transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, commandPool, device, graphicsQueue);

	//free unneeded resources
	PSIM_CORE_INFO("Created texture");
	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);

	return textureImage;
}

vk::ImageView VulkanTexture::createTextureImageView(vk::Image& textureImage, vk::Device& device)
{
	//create tex image view
	VulkanImage imageMaker;
	vk::ImageView textureImageView = imageMaker.createImageView(textureImage, device, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
	PSIM_CORE_INFO("Created Texture image view");

	return textureImageView;
}

vk::Sampler VulkanTexture::createTextureSampler(vk::Device& device)
{
	//define and create texture sampler
	vk::SamplerCreateInfo samplerInfo = { {}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
	vk::SamplerAddressMode::eRepeat, 0.0f, true, 16, false, vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, false };

	vk::Sampler textureSampler;
	PSIM_ASSERT(device.createSampler(&samplerInfo, nullptr, &textureSampler) == vk::Result::eSuccess, "Failed to create texture sampler!");

	return textureSampler;
}