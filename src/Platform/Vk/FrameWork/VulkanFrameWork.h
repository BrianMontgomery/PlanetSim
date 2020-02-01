#pragma once
#include "PSIMPCH.h"
#include "vulkan/vulkan.hpp"

#include "GLFW/glfw3.h"
#include "Platform/Vk/Modules/VulkanBuffer.h"

class VulkanFrameWork
{
public:
	VulkanFrameWork();
	~VulkanFrameWork();

	void init(GLFWwindow* window);
	void drawFrame(GLFWwindow* window);

	bool framebufferResized = false;

private:
	vk::Instance instance;
	vk::SurfaceKHR surface;

#ifdef PSIM_DEBUG
		vk::DebugUtilsMessengerEXT debugUtilsMessenger;
#endif

	vk::PhysicalDevice physicalDevice = nullptr;
	vk::Device device;
	vk::Queue graphicsQueue = nullptr;
	vk::Queue presentQueue = nullptr;

	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainExtent;

	std::vector<vk::ImageView> swapchainImageViews;

	vk::RenderPass renderPass;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;

	std::vector<vk::Framebuffer> swapchainFramebuffers;

	vk::Image depthImage;
	vk::DeviceMemory depthImageMemory;
	vk::ImageView depthImageView;

	vk::DescriptorSetLayout descriptorSetLayout;

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::Image textureImage;
	vk::DeviceMemory textureImageMemory;
	vk::ImageView textureImageView;
	vk::Sampler textureSampler;

	std::vector<VulkanBuffer::Vertex> vertices;
	vk::Buffer vertexBuffer;
	vk::DeviceMemory vertexBufferMemory;

	std::vector<uint32_t> indices;
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;

	std::vector<vk::Buffer> uniformBuffers;
	std::vector<vk::DeviceMemory> uniformBuffersMemory;

	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;
	std::vector<vk::Fence> imagesInFlight;
};

