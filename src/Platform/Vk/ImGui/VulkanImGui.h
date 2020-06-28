#pragma once
#include "Vk/FrameWork/VulkanFrameWork.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_vulkan.h>

#include "imgui/ImGuiLayer.h"

class VulkanImGui : public ImGuiPlatformLayer
{
public:
	VulkanImGui();
	~VulkanImGui() {};
	virtual void ImGuiOnAttach() override;
	virtual void ImGuiOnDetach() override;
	virtual void ImGuiOnUpdate(Timestep ts) override {};
	virtual void ImGuiOnImGuiRender() override;
	virtual void ImGuiOnEvent(Event& e) override;

	virtual void ImGuiBegin() override;
	virtual void ImGuiEnd() override;

private:
	VulkanFrameWork* framework;

	bool isImGuiWindowCreated;

	void ImGuiSetupWindow();
	void ImGuiBody();
};

