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

	SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(vk::PhysicalDevice& physicalDevice);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	vk::SwapchainKHR createSwapChain(GLFWwindow* window);
	void VulkanSwapchain::recreateSwapChain(GLFWwindow* window);
	void cleanupSwapChain();
	std::vector<vk::ImageView> createSwapchainImageViews();
	std::vector<vk::Framebuffer> createFramebuffers();

	std::vector<vk::Image> getSwapchainImages();
	vk::Format getSwapchainImageFormat();
	vk::Extent2D getSwapchainExtent();

private:
	std::vector<vk::Image> swapchainImages;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainExtent;
};

