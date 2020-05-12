#pragma once
#include "Platform/Vk/Modules/VulkanBuffer.h"

class VulkanModelLoad
{
public:
	VulkanModelLoad();
	~VulkanModelLoad();

	void loadModel(const std::string MODEL_PATH);
};

