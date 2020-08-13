#include "PSIMPCH.h"

#include "VulkanImGui.h"
#ifdef PSIM_DEBUG
#include "Application/Application.h"

VulkanImGui::VulkanImGui()
{
	framework = VulkanFrameWork::getFramework();
	isImGuiWindowCreated = false;
}

void VulkanImGui::ImGuiOnAttach()
{
	PSIM_PROFILE_FUNCTION();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	io.Fonts->AddFontFromFileTTF("C:\\dev\\PlanetSim\\assets\\fonts\\Roboto-Medium.ttf", 16.0f);
	io.DisplaySize = ImVec2(framework->getSwapChainExtent().width, framework->getSwapChainExtent().height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	Application& app = Application::Get();
	GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(window, true);

	VulkanFrameWork::QueueFamilyIndices imGuiIndices = framework->findQueueFamilies(framework->getPhysicalDevice());

	ImGui_ImplGlfw_InitForVulkan(window, false);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = ((VkInstance)framework->getInstance());
	init_info.PhysicalDevice = (VkPhysicalDevice)framework->getPhysicalDevice();
	init_info.Device = (VkDevice)framework->getDevice();
	init_info.QueueFamily = imGuiIndices.graphicsFamily.value();
	init_info.Queue = (VkQueue)framework->getPresentQueue();
	init_info.PipelineCache = framework->getPipelineCache();
	init_info.DescriptorPool = (VkDescriptorPool)framework->getDescriptorPool();
	init_info.Allocator = NULL;
	init_info.MinImageCount = 2;
	init_info.ImageCount = static_cast<uint32_t>(framework->getSwapChainImages()->size());
	init_info.CheckVkResultFn = NULL;
	init_info.MSAASamples = (VkSampleCountFlagBits)framework->getMSAASamples();
	ImGui_ImplVulkan_Init(&init_info, (VkRenderPass)framework->getRenderPass());

	vk::CommandBuffer commandBuffer = framework->beginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture((VkCommandBuffer)commandBuffer);
	framework->endSingleTimeCommands(commandBuffer);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void VulkanImGui::ImGuiOnDetach()
{
	PSIM_PROFILE_FUNCTION();

	framework->getDevice().waitIdle(); 
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void VulkanImGui::ImGuiOnImGuiRender()
{
	ImGuiBody();
}


void VulkanImGui::ImGuiOnEvent(Event& e)
{
	ImGuiIO& io = ImGui::GetIO();
	e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
	e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}


void VulkanImGui::ImGuiBegin()
{
	PSIM_PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO(); 
	Application& app = Application::Get();
	
	if (ImGuiResizeFlag)
	{
		reinitializeImGui();
		ImGuiResizeFlag = false;
	}

	if (!isImGuiWindowCreated)
	{
		ImGuiSetupWindow();
		isImGuiWindowCreated = true;
	}
}

void VulkanImGui::ImGuiEnd()
{
	PSIM_PROFILE_FUNCTION();
	
	// Rendering
	ImGui::Render();

	framework->getDevice().waitIdle();

	for (size_t i = 0; i < framework->getCommandBuffers()->size(); i++) {
		
		framework->commandBufferRecordBegin(i);

#ifdef PSIM_DEBUG
		if (isImGuiWindowCreated)
		{
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)(*framework->getCommandBuffers())[i]);
		}
#endif

		framework->commandBufferRecordEnd(i);
	}

	if (framework->framebufferResized == true)
	{
		ImGuiResizeFlag = true;
	}

	framework->drawFrame();

	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

	isImGuiWindowCreated = false;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void VulkanImGui::ImGuiSetupWindow()
{
		Application& app = Application::Get();
	if (!glfwWindowShouldClose(static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow())))
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		auto WindowSize = ImVec2((float)framework->getSwapChainExtent().width, (float)framework->getSwapChainExtent().height);
		ImGui::SetNextWindowSize(WindowSize, ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::NewFrame();
	}
}

void VulkanImGui::ImGuiBody()
{
	// render your GUI
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

}

void VulkanImGui::reinitializeImGui()
{
	framework->getDevice().waitIdle();
	ImGui_ImplVulkan_Shutdown();

	VulkanFrameWork::QueueFamilyIndices imGuiIndices = framework->findQueueFamilies(framework->getPhysicalDevice());

	Application& app = Application::Get();
	GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

	ImGui_ImplGlfw_InitForVulkan(window, false);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = (VkInstance)framework->getInstance();
	init_info.PhysicalDevice = (VkPhysicalDevice)framework->getPhysicalDevice();
	init_info.Device = (VkDevice)framework->getDevice();
	init_info.QueueFamily = imGuiIndices.graphicsFamily.value();
	init_info.Queue = (VkQueue)framework->getPresentQueue();
	init_info.PipelineCache = framework->getPipelineCache();
	init_info.DescriptorPool = (VkDescriptorPool)framework->getDescriptorPool();
	init_info.Allocator = NULL;
	init_info.MinImageCount = 2;
	init_info.ImageCount = static_cast<uint32_t>(framework->getSwapChainImages()->size());
	init_info.CheckVkResultFn = NULL;
	init_info.MSAASamples = (VkSampleCountFlagBits)framework->getMSAASamples();
	ImGui_ImplVulkan_Init(&init_info, (VkRenderPass)framework->getRenderPass());

	vk::CommandBuffer commandBuffer = framework->beginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture((VkCommandBuffer)commandBuffer);
	framework->endSingleTimeCommands(commandBuffer);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}
#endif

#ifndef PSIM_DEBUG
VulkanImGui::VulkanImGui()
{
	isImGuiWindowCreated = false;
}

void VulkanImGui::ImGuiOnAttach() {}
void VulkanImGui::ImGuiOnDetach() {}
void VulkanImGui::ImGuiOnImGuiRender() {}
void VulkanImGui::ImGuiOnEvent(Event& e) {}
void VulkanImGui::ImGuiBegin() {}
void VulkanImGui::ImGuiEnd() {}
void VulkanImGui::ImGuiSetupWindow() {}
void VulkanImGui::ImGuiBody() {}
#endif