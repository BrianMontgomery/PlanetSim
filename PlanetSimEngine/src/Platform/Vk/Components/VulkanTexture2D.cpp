#include "PSIMPCH.h"
#include "VulkanTexture2D.h"

#include <stb/stb_image.h>

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

VulkanTexture2D::VulkanTexture2D(std::string name, uint32_t width, uint32_t height)
	: m_TexWidth(width), m_TexHeight(height), m_Name(name)
{
	PSIM_PROFILE_FUNCTION();

	m_Format = vk::Format::eR8G8B8A8Srgb;
	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_TexWidth, m_TexHeight)))) + 1;
	vk::DeviceSize imageSize = m_TexWidth * m_TexHeight * 4;

	SetData(nullptr, imageSize);
}

VulkanTexture2D::VulkanTexture2D(const std::string& path)
	: m_Path(path)
{
	PSIM_PROFILE_FUNCTION();
	auto lastSlash = path.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = path.rfind('.');
	auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
	m_Name = path.substr(lastSlash, count);

	//load texture
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (texChannels == 3) {
		texChannels = 4;
	}

	vk::DeviceSize imageSize = texWidth * texHeight * texChannels;
	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	PSIM_ASSERT(pixels, "Failed to load texture image!");

	m_TexWidth = texWidth;
	m_TexHeight = texHeight;

	PSIM_ASSERT(pixels, "Failed to load texture image!");

	m_Format = vk::Format::eR8G8B8A8Srgb;
	if (texChannels == 4){
		m_Format = vk::Format::eR8G8B8A8Srgb;
	}

	PSIM_ASSERT(m_Format != vk::Format::eUndefined, "Format not supported!");

	SetData((void*)pixels, imageSize);
}

VulkanTexture2D::~VulkanTexture2D()
{
	
}

void VulkanTexture2D::destroy() const
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	framework->getDevice().destroyImageView(m_TextureImageView, nullptr);
	PSIM_CORE_INFO("Texture image view destroyed");

	framework->getDevice().destroySampler(m_TextureSampler, nullptr);
	PSIM_CORE_INFO("Texture sampler destroyed");

	framework->getDevice().destroyImage(m_TextureImage, nullptr);
	framework->getDevice().freeMemory(m_TextureImageMemory, nullptr);
	PSIM_CORE_INFO("Texture memory destroyed");

}

void VulkanTexture2D::SetData(void* pixels, uint32_t imageSize)
{
	PSIM_PROFILE_FUNCTION();

	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	framework->getBufferList()->getBaseBuffer()->createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	//transfer texture data to new buffer
	void* data;
	vkMapMemory(framework->getDevice(), framework->getBufferList()->getBaseBuffer()->getBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(framework->getDevice(), framework->getBufferList()->getBaseBuffer()->getBufferMemory());

	//unload image
	stbi_image_free(pixels);

	framework->createImage(m_TexWidth, m_TexHeight, m_MipLevels, vk::SampleCountFlagBits::e1, m_Format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, m_TextureImage, m_TextureImageMemory);

	//transition data format to optimal
	framework->transitionImageLayout(m_TextureImage, m_Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, m_MipLevels);
	framework->copyBufferToImage(framework->getBufferList()->getBaseBuffer()->getBuffer(), m_TextureImage, static_cast<uint32_t>(m_TexWidth), static_cast<uint32_t>(m_TexHeight));

	//free unneeded resources
	PSIM_CORE_INFO("Created texture");

	generateMipmaps(m_TextureImage, m_Format, m_TexWidth, m_TexHeight, m_MipLevels);

	createTextureImageView();
	createTextureSampler();
}

void VulkanTexture2D::Bind(uint32_t slot) const
{
	PSIM_PROFILE_FUNCTION();

	PSIMAssetLibraries* assetLibs = PSIMAssetLibraries::getAssetLibraries();

	PSIM_ASSERT(assetLibs->PSIM_TextureLibrary.getBoundTextures()->at(slot) == "", "Texture name slot: {0} is already full!", slot);
	assetLibs->PSIM_TextureLibrary.getBoundTextures()->at(slot) = m_Name;
}

//------------------------------------------------------------------------------------------------------------------------

void VulkanTexture2D::createTextureImageView()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//create tex image view
	m_TextureImageView = framework->createImageView(m_TextureImage, m_Format, vk::ImageAspectFlagBits::eColor, m_MipLevels);
	PSIM_CORE_INFO("Created Texture image view");
}

void VulkanTexture2D::createTextureSampler()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	//define and create texture sampler
	vk::SamplerCreateInfo samplerInfo = { {}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
	vk::SamplerAddressMode::eRepeat, 0.0f, true, 16, false, vk::CompareOp::eAlways, 0.0f, static_cast<float>(m_MipLevels), vk::BorderColor::eIntOpaqueBlack, false };

	PSIM_ASSERT(framework->getDevice().createSampler(&samplerInfo, nullptr, &m_TextureSampler) == vk::Result::eSuccess, "Failed to create texture sampler!");
}

void VulkanTexture2D::generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	// Check if image format supports linear blitting
	vk::FormatProperties formatProperties;
	framework->getPhysicalDevice().getFormatProperties(imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	vk::CommandBuffer commandBuffer = framework->beginSingleTimeCommands();

	vk::ImageMemoryBarrier barrier = { {}, {}, {}, {}, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1) };

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		commandBuffer.pipelineBarrier(vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eTransfer }, vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eTransfer },
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

		vk::ImageBlit blit = { vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1),
		{}, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1), {} };
		blit.srcOffsets[0] = vk::Offset3D(0, 0, 0);
		blit.srcOffsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
		blit.dstOffsets[0] = vk::Offset3D(0, 0, 0);
		blit.dstOffsets[1] = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1);

		commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, 1, &blit, vk::Filter::eLinear);

		barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;


		commandBuffer.pipelineBarrier(vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eTransfer }, vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eFragmentShader },
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eTransfer }, vk::PipelineStageFlags{ vk::PipelineStageFlagBits::eFragmentShader },
		vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	framework->endSingleTimeCommands(commandBuffer);
}