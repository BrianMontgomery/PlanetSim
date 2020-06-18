#pragma once

#include "Core/Layers/Layers.h"

#include "Core/Events/ApplicationEvents.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include "vulkan/vulkan.h"

class ImGuiPlatformLayer
{
public:
	virtual void ImGuiOnAttach() = 0;
	virtual void ImGuiOnDetach() = 0;
	virtual void ImGuiOnUpdate(Timestep ts) = 0;
	virtual void ImGuiOnImGuiRender() = 0;
	virtual void ImGuiOnEvent(Event& event) = 0;

	virtual void ImGuiBegin() = 0;
	virtual void ImGuiEnd() = 0;

	static Scope<ImGuiPlatformLayer> Create();
};


class ImGuiLayer : public Layers
{
public:
	ImGuiLayer();
	~ImGuiLayer() = default;

	virtual void OnAttach() override { platformLayer->ImGuiOnAttach(); }
	virtual void OnDetach() override { platformLayer->ImGuiOnDetach(); }
	virtual void OnUpdate(Timestep ts) override { platformLayer->ImGuiOnUpdate(ts); }
	virtual void OnImGuiRender() override { platformLayer->ImGuiOnImGuiRender(); }
	virtual void OnEvent(Event& event) override { platformLayer->ImGuiOnEvent(event); }

	void Begin() { platformLayer->ImGuiBegin(); }
	void End() { platformLayer->ImGuiEnd(); }



private:
	Scope<ImGuiPlatformLayer> platformLayer;
};