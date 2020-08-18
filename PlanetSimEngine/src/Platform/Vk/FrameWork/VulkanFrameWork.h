#pragma once
//in the premake but included here for redundancy

//vendor files
#define WIN32_LEAN_AND_MEAN
#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Graphics/Camera/Camera.h"
#include "Platform/Vk/Components/VulkanBuffer.h"
#include "Platform/Vk/Components/VulkanVertexArray.h"

#include "PSIM/Graphics/Asset/Model.h"

//non-core std-lib
#include <array>
#include <optional>

class VulkanFrameWork
{
	//singleton
	//--------------------------------------------------------------------------------------------------------------------------------
public:
	static VulkanFrameWork *getFramework();
protected: 
	VulkanFrameWork();
	virtual ~VulkanFrameWork();

	friend class VulkanFrameWorkDestroyer;

private:
	static VulkanFrameWork *m_framework;
	static VulkanFrameWorkDestroyer destroyer;

	//--------------------------------------------------------------------------------------------------------------------------------

	//nonsinglton
	//--------------------------------------------------------------------------------------------------------------------------------
public:
	bool framebufferResized = false;


	//public structs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return (graphicsFamily.has_value() && presentFamily.has_value());
		}
	};

	//--------------------------------------------------------------------------------------------------------------------------------


	//main vulkan funcs
	void init(GLFWwindow* window);
	void initVulkan();
	void drawFrame();
	void setViewMatrix(glm::mat4 VM) { viewMatrix = VM; }
	void setProjectionMatrix(glm::mat4 PM) { projMatrix = PM; }
	void setClearColor(glm::vec4 color) { clearColor = color; }
	void setMSAASamples(int sampleCount);

	//public vulkan funcs
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	void commandBufferRecordBegin(int bufNum);
	void commandBufferRecordEnd(int bufNum);
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

	//public vulkan getters
	vk::Device getDevice() { return device; }
	VulkanBufferList* getBufferList() { return &bufferList; }
	std::vector<vk::CommandBuffer>* getCommandBuffers() { return &commandBuffers; }
	int getCommandBuffersSize() { return commandBuffers.size(); }
	vk::Extent2D getSwapChainExtent() { return swapChainExtent; }
	vk::PhysicalDevice getPhysicalDevice() { return physicalDevice; }
	vk::Instance getInstance() { return instance; }
	vk::Queue getPresentQueue() { return presentQueue; }
	vk::PipelineCache getPipelineCache() { return pipelineCache; }
	vk::DescriptorPool getDescriptorPool() { return descriptorPool; }
	std::vector<vk::Image>* getSwapChainImages() { return &swapChainImages; }
	vk::SampleCountFlagBits getMSAASamples() { return msaaSamples; }
	vk::RenderPass getRenderPass() { return renderPass; }

	//--------------------------------------------------------------------------------------------------------------------------------

private:
	//private vars for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::vec4 clearColor;

	ModelLibrary m_StartingModelLibrary;

	GLFWwindow* window;
	VulkanBufferList bufferList;

	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugUtilsMessenger;

	std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	vk::SurfaceKHR surface = nullptr;

	vk::PhysicalDevice physicalDevice = nullptr;
	vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;

	vk::Device device;

	vk::Queue graphicsQueue;
	vk::Queue presentQueue;

	vk::SwapchainKHR swapChain;
	std::vector<vk::Image> swapChainImages;
	vk::Format swapChainImageFormat;
	vk::Extent2D swapChainExtent;

	std::vector<vk::ImageView> swapChainImageViews;

	vk::RenderPass renderPass;
	std::array<vk::ClearValue, 2> clearValues;
		
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;
	
	vk::PipelineCache pipelineCache;

	std::vector<vk::Framebuffer> swapChainFramebuffers;

	vk::Image colorImage;
	vk::DeviceMemory colorImageMemory;
	vk::ImageView colorImageView;

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::Image depthImage;
	vk::DeviceMemory depthImageMemory;
	vk::ImageView depthImageView;

	uint32_t mipLevels;
	vk::Image textureImage;
	vk::DeviceMemory textureImageMemory;
	vk::ImageView textureImageView;
	vk::Sampler textureSampler;

	Ref<VertexArray> vertexArray;

	vk::DescriptorSetLayout descriptorSetLayout;
	std::vector<VulkanBuffer> uniformBuffers;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;
	uint32_t drawFrameImageIndex;
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;
	std::vector<vk::Fence> imagesInFlight;
	//--------------------------------------------------------------------------------------------------------------------------------


	//private structs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	struct SwapChainSupportDetails {
		SwapChainSupportDetails(vk::SurfaceCapabilitiesKHR surfaceCapabilites, std::vector<vk::SurfaceFormatKHR> surfaceFormats, std::vector<vk::PresentModeKHR> surfacePresentModes)
			: capabilities(surfaceCapabilites), formats(surfaceFormats), presentModes(surfacePresentModes) {}
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	//--------------------------------------------------------------------------------------------------------------------------------

	//private funcs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	void cleanUp();

	void createInstance();
	std::vector<const char*> getInstanceExtensions();
	std::vector<const char*> getInstanceLayers();

	void setupDebugMessenger();
	vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

	void createSurface();

	void pickPhysicalDevice();
	bool isDeviceSuitable(vk::PhysicalDevice device);
	int rateDevice(vk::PhysicalDevice device);
	bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
	vk::SampleCountFlagBits getMaxUsableSampleCount();

	void createLogicalDevice();

	VulkanFrameWork::SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void recreateSwapChain();
	void cleanupSwapChain();

	void createSwapchainImageViews();
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLevels);

	void createRenderPass();
	void createGraphicsPipeline();
	vk::ShaderModule createShaderModule(const std::vector<char>& code);
	static std::vector<char> readFileByteCode(const std::string& filename);

	void createPipelineCache();
	void retrievePipelineCache();

	void createFramebuffers();

	void VulkanFrameWork::createColorResources();

	vk::CommandPool createCommandPool(vk::CommandPoolCreateFlags flags);
	void createCommandBuffers();

	void createDepthResources();
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	vk::Format findDepthFormat();
	bool hasStencilComponent(vk::Format format);

	void createTextureImage();
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
	void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	void createTextureImageView();
	void createTextureSampler();
	void generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	
	void createDescriptorSetLayout();
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool();
	void createDescriptorSets();

	void createSyncObjects();
	//--------------------------------------------------------------------------------------------------------------------------------
};

class VulkanFrameWorkDestroyer
{
public:
	VulkanFrameWorkDestroyer(VulkanFrameWork * = 0);
	~VulkanFrameWorkDestroyer() { delete _singleton; }
	void SetSingleton(VulkanFrameWork *s) { _singleton = s; }

private:
	VulkanFrameWork *_singleton;
};