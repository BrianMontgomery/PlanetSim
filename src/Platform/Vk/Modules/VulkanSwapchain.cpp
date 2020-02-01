#include "PSIMPCH.h"
#include "VulkanSwapchain.h"

#include "Platform/Vk/Modules/VulkanQueue.h"
#include "Platform/Vk/Modules/VulkanImage.h"
#include "Platform/Vk/Modules/VulkanPipeline.h"
#include "Platform/Vk/Modules/VulkanDepthBuffer.h"
#include "Platform/Vk/Modules/VulkanUniformBuffer.h"
#include "Platform/Vk/Modules/VulkanDescriptorSet.h"
#include "Platform/Vk/Modules/VulkanCommandBuffer.h"


VulkanSwapchain::VulkanSwapchain()
{
}


VulkanSwapchain::~VulkanSwapchain()
{
}

SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
{
	auto[result, surfaceCapabilities] = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get surface capabilities!");
	//formats

	auto[result1, surfaceFormats] = physicalDevice.getSurfaceFormatsKHR(surface);
	PSIM_ASSERT(result1 == vk::Result::eSuccess, "Failed to get surface formats!");

	//modes
	auto[result2, surfacePresentModes] = physicalDevice.getSurfacePresentModesKHR(surface);
	PSIM_ASSERT(result2 == vk::Result::eSuccess, "Failed to get surface present modes!");

	SwapChainSupportDetails details(surfaceCapabilities, surfaceFormats, surfacePresentModes);
	return details;
}

vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	//check for srgb
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	//check for mailbox mode
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
	//wanted extent
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	//fallback extent
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		vk::Extent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

vk::SwapchainKHR VulkanSwapchain::createSwapChain(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, GLFWwindow* window, vk::Device& device)
{
	//create/populate the swapChainSupport struct
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	//define an image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//create/populate a vkSwapChainInfo struct
	vk::SwapchainCreateInfoKHR createInfo = { {}, surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
	vk::SharingMode::eExclusive, 0, nullptr, swapChainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, true, nullptr };

	VulkanQueue vulkanQueue;
	VulkanQueue::QueueFamilyIndices indices = vulkanQueue.findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		createInfo.setQueueFamilyIndexCount(2);
		createInfo.setPQueueFamilyIndices(queueFamilyIndices);
	}

	//create the swapchain
	vk::SwapchainKHR swapchain;
	PSIM_ASSERT(device.createSwapchainKHR(&createInfo, nullptr, &swapchain) == vk::Result::eSuccess, "Failed to create swap chain!");
	PSIM_CORE_INFO("Created Swap Chain");

	//retrieving images
	auto[result, images] = device.getSwapchainImagesKHR(swapchain);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get swapChain Images!");
	swapchainImages = images;

	//storing vars for later
	swapchainImageFormat = (surfaceFormat.format);
	swapchainExtent = extent;

	return swapchain;
}

void VulkanSwapchain::recreateSwapChain(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface, GLFWwindow* window, vk::Device& device, vk::ImageView& depthImageView, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory, std::vector<vk::Framebuffer>& swapchainFramebuffers, vk::CommandPool& commandPool, std::vector<vk::CommandBuffer>& commandBuffers, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::RenderPass& renderPass, std::vector<vk::ImageView>& swapchainImageViews, vk::SwapchainKHR& swapchain, std::vector<vk::Buffer>& uniformBuffers, std::vector<vk::DeviceMemory>& uniformBuffersMemory, std::vector<vk::DescriptorSet>& descriptorSets, vk::DescriptorPool& descriptorPool, vk::DescriptorSetLayout& descriptorSetLayout, vk::Sampler& textureSampler, vk::ImageView& textureImageView, vk::Buffer& vertexBuffer, vk::Buffer& indexBuffer, std::vector<uint32_t>& indices)
{
	//check new window size and adapt
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	device.waitIdle();

	//clean swapchain specific resources
	cleanupSwapChain(device, depthImageView, depthImage, depthImageMemory, swapchainFramebuffers, commandPool, commandBuffers, graphicsPipeline, pipelineLayout, renderPass, swapchainImageViews, swapchain, uniformBuffers, uniformBuffersMemory, descriptorPool);

	VulkanPipeline pipelineMaker;
	VulkanDepthBuffer depthMaker;
	VulkanUniformBuffer uniformMaker;
	VulkanDescriptorSet descriptorMaker;
	VulkanCommandBuffer commandMaker;

	//recreate new swapchain specific resources
	swapchain = createSwapChain(physicalDevice, surface, window, device);
	swapchainImageViews = createSwapchainImageViews(device);
	renderPass = pipelineMaker.createRenderPass(swapchainImageFormat, physicalDevice, device);
	graphicsPipeline = pipelineMaker.createGraphicsPipeline(swapchainExtent, device, renderPass, descriptorSetLayout);
	pipelineLayout = pipelineMaker.getPipelineLayout();
	depthImageView = depthMaker.createDepthResources(physicalDevice, device, swapchainExtent, depthImage, depthImageMemory);
	swapchainFramebuffers = createFramebuffers(swapchainImageViews, renderPass, device, depthImageView);
	uniformBuffers = uniformMaker.createUniformBuffers(uniformBuffersMemory, swapchainImages, physicalDevice, device);
	descriptorPool = descriptorMaker.createDescriptorPool(swapchainImages, device);
	descriptorSets = descriptorMaker.createDescriptorSets(swapchainImages, descriptorSetLayout, descriptorPool, device, uniformBuffers, textureSampler, textureImageView);
	commandBuffers = commandMaker.createCommandBuffers(swapchainFramebuffers, commandPool, device, renderPass, swapchainExtent, graphicsPipeline, pipelineLayout, vertexBuffer, indexBuffer, descriptorSets, indices );

	PSIM_CORE_INFO("SwapChain recreated!");
}

void VulkanSwapchain::cleanupSwapChain(vk::Device& device, vk::ImageView& depthImageView, vk::Image& depthImage, vk::DeviceMemory& depthImageMemory, std::vector<vk::Framebuffer>& swapchainFramebuffers, vk::CommandPool& commandPool, std::vector<vk::CommandBuffer>& commandBuffers, vk::Pipeline& graphicsPipeline, vk::PipelineLayout& pipelineLayout, vk::RenderPass& renderPass, std::vector<vk::ImageView>& swapchainImageViews, vk::SwapchainKHR& swapchain, std::vector<vk::Buffer>& uniformBuffers, std::vector<vk::DeviceMemory>& uniformBuffersMemory, vk::DescriptorPool& descriptorPool)
{

	//clean swapcain specific resources
	device.destroyImageView(depthImageView, nullptr);
	device.destroyImage(depthImage, nullptr);
	device.freeMemory(depthImageMemory, nullptr);

	for (size_t i = 0; i < swapchainFramebuffers.size(); i++) {
		device.destroyFramebuffer(swapchainFramebuffers[i], nullptr);
	}

	device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	PSIM_CORE_INFO("Command Buffers freed");

	device.destroyPipeline(graphicsPipeline, nullptr);
	device.destroyPipelineLayout(pipelineLayout, nullptr);
	device.destroyRenderPass(renderPass, nullptr);
	PSIM_CORE_INFO("Pipeline deleted");

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		device.destroyImageView(swapchainImageViews[i], nullptr);
	}
	PSIM_CORE_INFO("ImageViews deleted");

	device.destroySwapchainKHR(swapchain, nullptr);
	PSIM_CORE_INFO("Swapchain cleaned up");

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		device.destroyBuffer(uniformBuffers[i], nullptr);
		device.freeMemory(uniformBuffersMemory[i], nullptr);
	}
	PSIM_CORE_INFO("Uniform Buffers cleaned up");

	device.destroyDescriptorPool(descriptorPool, nullptr);
	PSIM_CORE_INFO("Descriptor Pool cleaned up");
}

std::vector<vk::ImageView> VulkanSwapchain::createSwapchainImageViews(vk::Device& device)
{
	std::vector<vk::ImageView> swapchainImageViews;
	//get number of swapchain image views needed
	swapchainImageViews.resize(swapchainImages.size());

	//create them
	VulkanImage imageMaker;
	for (uint32_t i = 0; i < swapchainImages.size(); i++) {
		swapchainImageViews[i] = imageMaker.createImageView(swapchainImages[i], device, swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
	}
	PSIM_CORE_INFO("Created swapchain image views");

	return swapchainImageViews;
}

std::vector<vk::Framebuffer> VulkanSwapchain::createFramebuffers(std::vector<vk::ImageView>& swapchainImageViews, vk::RenderPass& renderPass, vk::Device& device, vk::ImageView& depthImageView)
{
	//get number of framebuffers
	std::vector<vk::Framebuffer> swapchainFramebuffers;
	swapchainFramebuffers.resize(swapchainImageViews.size());

	//create framebuffers
	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			swapchainImageViews[i],
			depthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = { {}, renderPass, static_cast<uint32_t>(attachments.size()), attachments.data(), swapchainExtent.width, swapchainExtent.height, 1 };

		PSIM_ASSERT(device.createFramebuffer(&framebufferInfo, nullptr, &swapchainFramebuffers[i]) == vk::Result::eSuccess, "Failed to create framebuffer!");
	}
	PSIM_CORE_INFO("Framebuffers created");

	return swapchainFramebuffers;
}

std::vector<vk::Image> VulkanSwapchain::getSwapchainImages() 
{
	return swapchainImages;
}

vk::Format VulkanSwapchain::getSwapchainImageFormat()
{
	return swapchainImageFormat;
}

vk::Extent2D VulkanSwapchain::getSwapchainExtent()
{
	return swapchainExtent;
}