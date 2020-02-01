#pragma once
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

struct SwapChainSupportDetails {
	SwapChainSupportDetails(vk::SurfaceCapabilitiesKHR surfaceCapabilites, std::vector<vk::SurfaceFormatKHR> surfaceFormats, std::vector<vk::PresentModeKHR> surfacePresentModes)
		: capabilities(surfaceCapabilites), formats(surfaceFormats), presentModes(surfacePresentModes) {}
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

class VulkanSwapchain
{
public:
	VulkanSwapchain();
	~VulkanSwapchain();

	SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	vk::SwapchainKHR createSwapChain(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, GLFWwindow* window, vk::Device& device);
	void VulkanSwapchain::recreateSwapChain(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, GLFWwindow* window, vk::Device& device, vk::ImageView& depthImageView, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory, std::vector<vk::Framebuffer>& swapchainFramebuffers, vk::CommandPool& commandPool, std::vector<vk::CommandBuffer>& commandBuffers, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::RenderPass& renderPass, std::vector<vk::ImageView>& swapchainImageViews, vk::SwapchainKHR& swapchain, std::vector<vk::Buffer>& uniformBuffers, std::vector<vk::DeviceMemory>& uniformBuffersMemory, std::vector<vk::DescriptorSet>& descriptorSets, vk::DescriptorPool& descriptorPool, vk::DescriptorSetLayout& descriptorSetLayout, vk::Sampler& textureSampler, vk::ImageView& textureImageView, vk::Buffer& vertexBuffer, vk::Buffer& indexBuffer, std::vector<uint32_t>& indices);
	void cleanupSwapChain(vk::Device& device, vk::ImageView& depthImageView, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory, std::vector<vk::Framebuffer>& swapchainFramebuffers, vk::CommandPool& commandPool, std::vector<vk::CommandBuffer>& commandBuffers, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::RenderPass& renderPass, std::vector<vk::ImageView>& swapchainImageViews, vk::SwapchainKHR& swapchain, std::vector<vk::Buffer>& uniformBuffers, std::vector<vk::DeviceMemory>& uniformBuffersMemory, vk::DescriptorPool& descriptorPool);
	std::vector<vk::ImageView> createSwapchainImageViews(vk::Device& device);
	std::vector<vk::Framebuffer> createFramebuffers(std::vector<vk::ImageView>& swapchainImageViews, vk::RenderPass& renderPass, vk::Device& device, vk::ImageView& depthImageView);

	std::vector<vk::Image> getSwapchainImages();
	vk::Format getSwapchainImageFormat();
	vk::Extent2D getSwapchainExtent();

private:
	std::vector<vk::Image> swapchainImages;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainExtent;
};

