#include "PSIMPCH.h"
#include "imgui/ImGuiLayer.h"
#include <imgui.h>

#include "Vk/ImGui/VulkanImGui.h"
#include "Graphics/Renderer.h"


// TEMPORARY
#include <GLFW/glfw3.h>

ImGuiLayer::ImGuiLayer()
	: Layers("ImGuiLayer")
{
	platformLayer = ImGuiPlatformLayer::Create();
}

Scope<ImGuiPlatformLayer> ImGuiPlatformLayer::Create()
{
	PSIM_PROFILE_FUNCTION();
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:    PSIM_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:  PSIM_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
	case RendererAPI::API::Vulkan:  return CreateScope<VulkanImGui>();
	}

	PSIM_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}