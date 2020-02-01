#include "PSIMPCH.h"
#include "VulkanFrameWork.h"


#include "Platform/Vk/Modules/VulkanInstance.h"			//--also includes the surface--//
#ifdef PSIM_DEBUG
	#include "Platform/Vk/Modules/VulkanDebugger.h"
#endif
#include "Platform/Vk/Modules/VulkanDevice.h"
#include "Platform/Vk/Modules/VulkanSwapchain.h"
#include "Platform/Vk/Modules/VulkanPipeline.h"
#include "Platform/Vk/Modules/VulkanDescriptorSet.h"
#include "Platform/Vk/Modules/VulkanDepthBuffer.h"
#include "Platform/Vk/Modules/VulkanCommandBuffer.h"
#include "Platform/Vk/Modules/VulkanTexture.h"
#include "Platform/Vk/Modules/VulkanModelLoad.h"
#include "Platform/Vk/Modules/VulkanBuffer.h"
#include "Platform/Vk/Modules/VulkanUniformBuffer.h"
#include "Platform/Vk/Modules/VulkanSync.h"

const char* TEXTURE_PATH = "resources/textures/chalet.jpg";
const std::string MODEL_PATH = "resources/models/chalet.obj";

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	//get new size of window
	auto app = reinterpret_cast<VulkanFrameWork*>(glfwGetWindowUserPointer(window));

	//tell app resize has happened
	app->framebufferResized = true;
}

VulkanFrameWork::VulkanFrameWork()
{
}

VulkanFrameWork::~VulkanFrameWork()
{
	VulkanSwapchain swapchainDestroy;
	swapchainDestroy.cleanupSwapChain(device, depthImageView, depthImage, depthImageMemory, swapchainFramebuffers, commandPool, commandBuffers, graphicsPipeline, pipelineLayout, renderPass, swapchainImageViews, swapchain, uniformBuffers, uniformBuffersMemory, descriptorPool);

	device.destroySampler(textureSampler, nullptr);
	PSIM_CORE_INFO("Texture sampler destroyed");

	device.destroyImageView(textureImageView, nullptr);
	PSIM_CORE_INFO("Texture image view destroyed");

	device.destroyImage(textureImage, nullptr);
	device.freeMemory(textureImageMemory, nullptr);
	PSIM_CORE_INFO("Image destroyed");

	device.destroyDescriptorSetLayout(descriptorSetLayout, nullptr);

	device.destroyBuffer(indexBuffer, nullptr);
	device.freeMemory(indexBufferMemory, nullptr);
	PSIM_CORE_INFO("Index Buffer Destroyed");

	device.destroyBuffer(vertexBuffer, nullptr);
	device.freeMemory(vertexBufferMemory, nullptr);
	PSIM_CORE_INFO("Vertex Buffer Destroyed");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		device.destroySemaphore(renderFinishedSemaphores[i], nullptr);
		device.destroySemaphore(imageAvailableSemaphores[i], nullptr);
		device.destroyFence(inFlightFences[i], nullptr);
	}
	PSIM_CORE_INFO("Sync Objects deleted");

	device.destroyCommandPool(commandPool, nullptr);
	PSIM_CORE_INFO("Command Pool deleted");

	device.destroy(nullptr);
	PSIM_CORE_INFO("Device deleted");

#ifdef PSIM_DEBUG
		instance.destroyDebugUtilsMessengerEXT(debugUtilsMessenger, nullptr);
		PSIM_CORE_INFO("Debug Messenger deleted");
#endif

	instance.destroySurfaceKHR(surface, nullptr);
	PSIM_CORE_INFO("Surface deleted");

	instance.destroy(nullptr);
	PSIM_CORE_INFO("Vulkan instance deleted");
}

void VulkanFrameWork::init(GLFWwindow* window)
{
	//setup instance
	VulkanInstance instanceMaker;
	instance = instanceMaker.createNewInstance();

	//if nDebug setup debug messenger
#ifdef PSIM_DEBUG
	{
		VulkanDebugger debuggerMaker;
		debugUtilsMessenger = debuggerMaker.createNewDebugger(instance);
	}
#endif

	VulkanSurface surfaceMaker;
	surface = surfaceMaker.createNewSurface(window, instance);

	VulkanDevice deviceMaker;
	physicalDevice = deviceMaker.pickPhysicalDevice(instance, surface);
	device = deviceMaker.createLogicalDevice(physicalDevice, surface, instanceMaker.getLayers(), graphicsQueue, presentQueue);

	VulkanSwapchain swapchainMaker;
	swapchain = swapchainMaker.createSwapChain(physicalDevice, surface, window, device);
	swapchainImageViews = swapchainMaker.createSwapchainImageViews(device);
	swapchainImages = swapchainMaker.getSwapchainImages();
	swapchainImageFormat = swapchainMaker.getSwapchainImageFormat();
	swapchainExtent = swapchainMaker.getSwapchainExtent();

	VulkanPipeline pipelineMaker;
	renderPass = pipelineMaker.createRenderPass(swapchainImageFormat, physicalDevice, device);

	VulkanDescriptorSet descriptorMaker;
	descriptorSetLayout = descriptorMaker.createDescriptorSetLayout(device);

	graphicsPipeline = pipelineMaker.createGraphicsPipeline(swapchainExtent, device, renderPass, descriptorSetLayout);
	pipelineLayout = pipelineMaker.getPipelineLayout();

	VulkanDepthBuffer depthBufferMaker;
	depthImageView = depthBufferMaker.createDepthResources(physicalDevice, device, swapchainExtent, depthImage, depthImageMemory);

	swapchainFramebuffers = swapchainMaker.createFramebuffers(swapchainImageViews, renderPass, device, depthImageView);

	VulkanCommandBuffer commandBufferMaker;
	commandPool = commandBufferMaker.createCommandPool(physicalDevice, surface, device);

	VulkanTexture textureMaker;
	textureImage = textureMaker.createTextureImage(TEXTURE_PATH, commandPool, device, graphicsQueue, physicalDevice, textureImageMemory);
	textureImageView = textureMaker.createTextureImageView(textureImage, device);
	textureSampler = textureMaker.createTextureSampler(device);

	VulkanModelLoad modelLoader;
	modelLoader.loadModel(MODEL_PATH, vertices, indices);

	VulkanBuffer bufferMaker;
	vertexBuffer = bufferMaker.createVertexBuffer(vertices, vertexBufferMemory, physicalDevice, device, commandPool, graphicsQueue);
	indexBuffer = bufferMaker.createIndexBuffer(indexBufferMemory, physicalDevice, device, indices, commandPool, graphicsQueue);

	VulkanUniformBuffer uniformMaker;
	uniformBuffers = uniformMaker.createUniformBuffers(uniformBuffersMemory, swapchainImages, physicalDevice, device);

	descriptorPool = descriptorMaker.createDescriptorPool(swapchainImages, device);
	descriptorSets = descriptorMaker.createDescriptorSets(swapchainImages, descriptorSetLayout, descriptorPool, device, uniformBuffers, textureSampler, textureImageView);

	commandBuffers = commandBufferMaker.createCommandBuffers(swapchainFramebuffers, commandPool, device, renderPass, swapchainExtent, graphicsPipeline, pipelineLayout, vertexBuffer, indexBuffer, descriptorSets, indices);
	
	VulkanSync syncMaker;
	syncMaker.createSyncObjects(MAX_FRAMES_IN_FLIGHT, currentFrame, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences, imagesInFlight, swapchainImages, device);
	PSIM_CORE_INFO("Vulkan initialization complete");
}

void VulkanFrameWork::drawFrame(GLFWwindow* window)
{
	//ensure that all previous draws are completed
	device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//get next frame
	uint32_t imageIndex;
	vk::Result result = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], vk::Fence(), &imageIndex);

	//check for resize
	VulkanSwapchain swapchainMaker;
	if (result == vk::Result::eErrorOutOfDateKHR) {
		swapchainMaker.recreateSwapChain(physicalDevice, surface, window, device, depthImageView, depthImage, depthImageMemory, swapchainFramebuffers, commandPool, commandBuffers, graphicsPipeline, pipelineLayout, renderPass, swapchainImageViews, swapchain, uniformBuffers, uniformBuffersMemory, descriptorSets, descriptorPool, descriptorSetLayout, textureSampler, textureImageView, vertexBuffer, indexBuffer, indices);
		return;
	}
	else {
		PSIM_ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "Failed to acquire swap chain image!");
	}

	//update to next frame
	VulkanUniformBuffer uniformUpdater;
	uniformUpdater.updateUniformBuffer(imageIndex, swapchainExtent, device, uniformBuffersMemory);

	if (bool(imagesInFlight[imageIndex]) != false) {
		device.waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//setup frame submission info

	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	vk::SubmitInfo submitInfo = { 1, waitSemaphores, waitStages, 1, &commandBuffers[imageIndex], 1, signalSemaphores };

	//reset sync objects when complete
	device.resetFences(1, &inFlightFences[currentFrame]);

	//push completed frame
	PSIM_ASSERT(graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]) == vk::Result::eSuccess, "Failed to submit draw command buffer!");

	//info for frame submission
	vk::SwapchainKHR swapChains[] = { swapchain };
	vk::PresentInfoKHR presentInfo = { 1, signalSemaphores, 1, swapChains, &imageIndex };

	//present frame
	result = presentQueue.presentKHR(&presentInfo);

	//check if frame was presented
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
		framebufferResized = false;
		swapchainMaker.recreateSwapChain(physicalDevice, surface, window, device, depthImageView, depthImage, depthImageMemory, swapchainFramebuffers, commandPool, commandBuffers, graphicsPipeline, pipelineLayout, renderPass, swapchainImageViews, swapchain, uniformBuffers, uniformBuffersMemory, descriptorSets, descriptorPool, descriptorSetLayout, textureSampler, textureImageView, vertexBuffer, indexBuffer, indices);
	}
	else {
		PSIM_ASSERT(result == vk::Result::eSuccess, "Failed to present swap chain image!");
	}

	//get next frame
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}