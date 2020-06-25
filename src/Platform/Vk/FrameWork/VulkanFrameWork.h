#pragma once
//in the premake but included here for redundancy

//vendor files
#define WIN32_LEAN_AND_MEAN
#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

//non-core std-lib
#include <array>
#include <optional>


class VulkanFrameWork
{
public:
	static VulkanFrameWork *getFramework() {
		if (!m_framework)
			m_framework = new VulkanFrameWork;
		return m_framework;
	}

	//public var for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	bool framebufferResized = false;

	//--------------------------------------------------------------------------------------------------------------------------------


	//public structs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
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

	//--------------------------------------------------------------------------------------------------------------------------------


	//public funcs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	~VulkanFrameWork();

	//main vulkan funcs
	void init(GLFWwindow* window);
	void drawFrame();

	//bool mainLoop();

	//--------------------------------------------------------------------------------------------------------------------------------

private:
	//private vars for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	
	GLFWwindow* window;
	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugUtilsMessenger;

	std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	vk::SurfaceKHR surface = nullptr;

	vk::PhysicalDevice physicalDevice = nullptr;
	vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e8;

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

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vk::Buffer vertexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;

	vk::DescriptorSetLayout descriptorSetLayout;
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
	//--------------------------------------------------------------------------------------------------------------------------------


	//private structs for vulkan
	//--------------------------------------------------------------------------------------------------------------------------------
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return (graphicsFamily.has_value() && presentFamily.has_value());
		}
	};

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
	void initVulkan();
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

	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

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

	void loadModel();

	void createVertexBuffer();
	void createIndexBuffer();
	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

	void createDescriptorSetLayout();
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool();
	void createDescriptorSets();

	void createSyncObjects();

	void commandBufferRecordBegin(int bufNum);
	void commandBufferRecordEnd(int bufNum);
	//--------------------------------------------------------------------------------------------------------------------------------

private:
	//singleton funcs
	VulkanFrameWork();

	static VulkanFrameWork *m_framework;

#ifdef PSIM_DEBUG
	friend class VulkanImGui;
#endif
#ifndef PSIM_DEBUG
	friend class VulkanContext;
#endif
};