#pragma once
#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"

class VulkanUniformBuffer
{
public:
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	VulkanUniformBuffer();
	~VulkanUniformBuffer();

	std::vector<vk::Buffer> createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
};

