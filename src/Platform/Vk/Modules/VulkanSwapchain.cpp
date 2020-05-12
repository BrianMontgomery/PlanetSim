#include "PSIMPCH.h"
#include "VulkanSwapchain.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
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

SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(vk::PhysicalDevice& physicalDevice)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	auto[result, surfaceCapabilities] = physicalDevice.getSurfaceCapabilitiesKHR(framework->surface);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get surface capabilities!");
	//formats

	auto[result1, surfaceFormats] = physicalDevice.getSurfaceFormatsKHR(framework->surface);
	PSIM_ASSERT(result1 == vk::Result::eSuccess, "Failed to get surface formats!");

	//modes
	auto[result2, surfacePresentModes] = physicalDevice.getSurfacePresentModesKHR(framework->surface);
	PSIM_ASSERT(result2 == vk::Result::eSuccess, "Failed to get surface present modes!");

	SwapChainSupportDetails details(surfaceCapabilities, surfaceFormats, surfacePresentModes);
	return details;
}

vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	PSIM_PROFILE_FUNCTION();

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
	PSIM_PROFILE_FUNCTION();

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
	PSIM_PROFILE_FUNCTION();

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

vk::SwapchainKHR VulkanSwapchain::createSwapChain(GLFWwindow* window)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//create/populate the swapChainSupport struct
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(framework->physicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	//define an image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//create/populate a vkSwapChainInfo struct
	vk::SwapchainCreateInfoKHR createInfo = { {}, framework->surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
	vk::SharingMode::eExclusive, 0, nullptr, swapChainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, true, nullptr };

	VulkanQueue vulkanQueue;
	VulkanQueue::QueueFamilyIndices indices = vulkanQueue.findQueueFamilies(framework->physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		createInfo.setQueueFamilyIndexCount(2);
		createInfo.setPQueueFamilyIndices(queueFamilyIndices);
	}

	//create the swapchain
	vk::SwapchainKHR swapchain;
	PSIM_ASSERT(framework->device.createSwapchainKHR(&createInfo, nullptr, &swapchain) == vk::Result::eSuccess, "Failed to create swap chain!");
	PSIM_CORE_INFO("Created Swap Chain");

	//retrieving images
	auto[result, images] = framework->device.getSwapchainImagesKHR(swapchain);
	PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to get swapChain Images!");
	swapchainImages = images;

	//storing vars for later
	swapchainImageFormat = (surfaceFormat.format);
	swapchainExtent = extent;

	return swapchain;
}

void VulkanSwapchain::recreateSwapChain(GLFWwindow* window)
{
	PSIM_PROFILE_FUNCTION()
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	//check new window size and adapt
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	framework->device.waitIdle();

	//clean swapchain specific resources
	cleanupSwapChain();

	VulkanPipeline pipelineMaker;
	VulkanDepthBuffer depthMaker;
	VulkanUniformBuffer uniformMaker;
	VulkanDescriptorSet descriptorMaker;
	VulkanCommandBuffer commandMaker;

	//recreate new swapchain specific resources
	framework->swapchain = createSwapChain(window);
	framework->swapchainImageViews = createSwapchainImageViews();
	framework->renderPass = pipelineMaker.createRenderPass();
	framework->graphicsPipeline = pipelineMaker.createGraphicsPipeline();
	framework->pipelineLayout = pipelineMaker.getPipelineLayout();
	framework->depthImageView = depthMaker.createDepthResources();
	framework->swapchainFramebuffers = createFramebuffers();
	framework->uniformBuffers = uniformMaker.createUniformBuffers();
	framework->descriptorPool = descriptorMaker.createDescriptorPool();
	framework->descriptorSets = descriptorMaker.createDescriptorSets();
	framework->commandBuffers = commandMaker.createCommandBuffers();

	PSIM_CORE_INFO("SwapChain recreated!");
}

void VulkanSwapchain::cleanupSwapChain()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//clean swapcain specific resources
	framework->device.destroyImageView(framework->depthImageView, nullptr);
	framework->device.destroyImage(framework->depthImage, nullptr);
	framework->device.freeMemory(framework->depthImageMemory, nullptr);

	for (size_t i = 0; i < framework->swapchainFramebuffers.size(); i++) {
		framework->device.destroyFramebuffer(framework->swapchainFramebuffers[i], nullptr);
	}

	framework->device.freeCommandBuffers(framework->commandPool, static_cast<uint32_t>(framework->commandBuffers.size()), framework->commandBuffers.data());
	PSIM_CORE_INFO("Command Buffers freed");

	framework->device.destroyPipeline(framework->graphicsPipeline, nullptr);
	framework->device.destroyPipelineLayout(framework->pipelineLayout, nullptr);
	framework->device.destroyRenderPass(framework->renderPass, nullptr);
	PSIM_CORE_INFO("Pipeline deleted");

	for (size_t i = 0; i < framework->swapchainImageViews.size(); i++) {
		framework->device.destroyImageView(framework->swapchainImageViews[i], nullptr);
	}
	PSIM_CORE_INFO("ImageViews deleted");

	framework->device.destroySwapchainKHR(framework->swapchain, nullptr);
	PSIM_CORE_INFO("Swapchain cleaned up");

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		framework->device.destroyBuffer(framework->uniformBuffers[i], nullptr);
		framework->device.freeMemory(framework->uniformBuffersMemory[i], nullptr);
	}
	PSIM_CORE_INFO("Uniform Buffers cleaned up");

	framework->device.destroyDescriptorPool(framework->descriptorPool, nullptr);
	PSIM_CORE_INFO("Descriptor Pool cleaned up");
}

std::vector<vk::ImageView> VulkanSwapchain::createSwapchainImageViews()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	std::vector<vk::ImageView> swapchainImageViews;
	//get number of swapchain image views needed
	swapchainImageViews.resize(swapchainImages.size());

	//create them
	VulkanImage imageMaker;
	for (uint32_t i = 0; i < swapchainImages.size(); i++) {
		swapchainImageViews[i] = imageMaker.createImageView(swapchainImages[i], framework->swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
	}
	PSIM_CORE_INFO("Created swapchain image views");

	return swapchainImageViews;
}

std::vector<vk::Framebuffer> VulkanSwapchain::createFramebuffers()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//get number of framebuffers
	std::vector<vk::Framebuffer> swapchainFramebuffers;
	swapchainFramebuffers.resize(framework->swapchainImageViews.size());

	//create framebuffers
	for (size_t i = 0; i < framework->swapchainImageViews.size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			framework->swapchainImageViews[i],
			framework->depthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = { {}, framework->renderPass, static_cast<uint32_t>(attachments.size()), attachments.data(), swapchainExtent.width, swapchainExtent.height, 1 };

		PSIM_ASSERT(framework->device.createFramebuffer(&framebufferInfo, nullptr, &swapchainFramebuffers[i]) == vk::Result::eSuccess, "Failed to create framebuffer!");
	}
	PSIM_CORE_INFO("Framebuffers created");

	return swapchainFramebuffers;
}

std::vector<vk::Image> VulkanSwapchain::getSwapchainImages() 
{
	PSIM_PROFILE_FUNCTION();

	return swapchainImages;
}

vk::Format VulkanSwapchain::getSwapchainImageFormat()
{
	PSIM_PROFILE_FUNCTION();

	return swapchainImageFormat;
}

vk::Extent2D VulkanSwapchain::getSwapchainExtent()
{
	PSIM_PROFILE_FUNCTION();

	return swapchainExtent;
}