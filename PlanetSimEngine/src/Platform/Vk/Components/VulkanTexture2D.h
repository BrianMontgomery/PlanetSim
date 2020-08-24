#pragma once
#include "PSIM/Graphics/Asset/Texture.h"

#include "vulkan/vulkan.hpp"


class VulkanTexture2D : public Texture2D
{
public:
	VulkanTexture2D(std::string name, uint32_t width, uint32_t height);
	VulkanTexture2D(const std::string& path);
	virtual ~VulkanTexture2D();

	virtual const std::string& GetName() const override { return m_Name; };
	virtual uint32_t GetWidth() const override { return m_TexWidth; }
	virtual uint32_t GetHeight() const override { return m_TexHeight; }
	virtual uint32_t GetRendererID() const override { return m_RendererID; }
	virtual uint32_t GetMipLevels() const override { return m_MipLevels; }
	virtual void* GetImageView() const override { return (void*)&m_TextureImageView; }

	virtual void SetData(void* data, uint32_t imageSize) override;

	virtual void Bind(uint32_t slot = 0) const override;

	virtual bool operator==(const Texture& other) const override
	{
		return m_RendererID == ((VulkanTexture2D&)other).m_RendererID;
	}
private:
	std::string m_Path;
	std::string m_Name;
	uint32_t m_TexWidth, m_TexHeight;
	uint32_t m_RendererID;
	uint32_t m_MipLevels;
	vk::Format m_Format;
	vk::Image m_TextureImage;
	vk::DeviceMemory m_TextureImageMemory;
	vk::ImageView m_TextureImageView;

	void createTextureImageView();
	void generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
};
