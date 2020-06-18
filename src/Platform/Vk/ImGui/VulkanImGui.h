#pragma once
#include "Vk/FrameWork/VulkanFrameWork.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_vulkan.h>

#include "imgui/ImGuiLayer.h"

class VulkanImGui : public ImGuiPlatformLayer
{
public:
	VulkanImGui();
	virtual void ImGuiOnAttach() override;
	virtual void ImGuiOnDetach() override;
	virtual void ImGuiOnUpdate(Timestep ts) override {};
	virtual void ImGuiOnImGuiRender() override {};
	virtual void ImGuiOnEvent(Event& e) override;

	virtual void ImGuiBegin() override;
	virtual void ImGuiEnd() override;

	void initResources(vk::RenderPass renderPass, vk::Queue copyQueue);
	void createImGuiCommandBuffers();

	struct PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;

private:
	int lastWindowWidth, lastWindowHeight;
	int imGuiWidth, imGuiHeight;

	ImVec4 clearValue = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	VulkanFrameWork* framework;

	vk::CommandPool imGuiCommandPool;
	std::array<vk::CommandBuffer, 2> imGuiCommandBuffers;
	
	vk::Sampler sampler;

	vk::Buffer vertexBuffer;
	void* vertexData = nullptr;
	vk::DeviceMemory vertexMemory;
	vk::DeviceSize vertexSize;
	vk::DescriptorBufferInfo vertexDescriptor;
	int32_t vertexCount = 0;

	vk::Buffer indexBuffer;
	void* indexData = nullptr;
	vk::DeviceMemory indexMemory;
	vk::DeviceSize indexSize;
	vk::DescriptorBufferInfo indexDescriptor;
	int32_t indexCount = 0;

	vk::DeviceMemory fontMemory = nullptr;
	vk::Image fontImage = nullptr;
	vk::ImageView fontView = nullptr;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline pipeline;
	vk::DescriptorPool descriptorPool;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorSet descriptorSet;

	struct UISettings {
		bool displayModels = true;
		bool displayLogos = true;
		bool displayBackground = true;
		bool animateLight = false;
		float lightSpeed = 0.25f;
		std::array<float, 50> frameTimes{};
		float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
		float lightTimer = 0.0f;
	} uiSettings;
};

