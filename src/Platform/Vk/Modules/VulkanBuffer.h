#pragma once
#include "vulkan/vulkan.hpp"

#include <glm/glm.hpp>

class VulkanBuffer
{
public:
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static vk::VertexInputBindingDescription getBindingDescription() {
			vk::VertexInputBindingDescription bindingDescription = { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };

			return bindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {};

			attributeDescriptions[0] = { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) };
			attributeDescriptions[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) };
			attributeDescriptions[2] = { 2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord) };
			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	VulkanBuffer();
	~VulkanBuffer();

	//create buffer
	void createBuffer(vk::DeviceSize& size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);

	//create vertex buffer
	vk::Buffer VulkanBuffer::createVertexBuffer();

	//index buffer
	vk::Buffer VulkanBuffer::createIndexBuffer();
};

