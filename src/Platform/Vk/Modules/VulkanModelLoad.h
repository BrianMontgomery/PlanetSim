#pragma once
#include "Platform/Vk/Modules/VulkanBuffer.h"

class VulkanModelLoad
{
public:
	VulkanModelLoad();
	~VulkanModelLoad();

	void loadModel(const std::string MODEL_PATH, std::vector<VulkanBuffer::Vertex>& vertices, std::vector<uint32_t>& indices);
};

