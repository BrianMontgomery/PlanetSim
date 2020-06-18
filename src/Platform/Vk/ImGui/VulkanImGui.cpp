#include "PSIMPCH.h"
#include "VulkanImGui.h"
#include "imgui.h"

#include "Application/Application.h"

static void check_vk_result(VkResult err)
{
	PSIM_ASSERT(err == 0, "ImGui Vulkan Error: {0}", err);
}

VulkanImGui::VulkanImGui()
{
	framework = VulkanFrameWork::getFramework();
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
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	io.Fonts->AddFontDefault();

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

	//initialize imgui for vulkan
	ImGui_ImplGlfw_InitForVulkan(framework->window, true);

	//create init info
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = (VkInstance)framework->instance;
	init_info.PhysicalDevice = (VkPhysicalDevice)framework->physicalDevice;
	init_info.Device = (VkDevice)framework->device;

	VulkanFrameWork::QueueFamilyIndices indices = framework->findQueueFamilies(framework->physicalDevice);
	init_info.QueueFamily = indices.graphicsFamily.value();

	init_info.Queue = (VkQueue)framework->graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = (VkDescriptorPool)framework->descriptorPool;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.MinImageCount = 2;
	init_info.ImageCount = framework->swapChainImages.size();
	init_info.MSAASamples = (VkSampleCountFlagBits)framework->msaaSamples;
	init_info.CheckVkResultFn = check_vk_result;

	imGuiCommandPool = framework->createCommandPool({ vk::CommandPoolCreateFlagBits::eTransient });
	initResources(framework->renderPass, framework->graphicsQueue);
	//initialize vulkan resources in ImGui
	ImGui_ImplVulkan_Init(&init_info, (VkRenderPass)framework->renderPass);
}

void VulkanImGui::ImGuiOnDetach()
{
	PSIM_PROFILE_FUNCTION();

	framework->device.destroyCommandPool(imGuiCommandPool, nullptr);
	PSIM_CORE_INFO("Command Pool deleted");

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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

	if (lastWindowWidth != framework->swapChainExtent.width && lastWindowHeight != framework->swapChainExtent.height && framework->swapChainExtent.width > 0 && framework->swapChainExtent.width > 0)
	{
		ImGui_ImplVulkan_SetMinImageCount(framework->swapChainImages.size());
		//createOrResizeImGuiWindow();
	}

	lastWindowWidth = framework->swapChainExtent.width;
	lastWindowHeight = framework->swapChainExtent.height;

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void VulkanImGui::ImGuiEnd()
{
	PSIM_PROFILE_FUNCTION();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)framework->swapChainExtent.width, (float)framework->swapChainExtent.height);

	ImVec4 clear_color = ImColor(114, 144, 154);
	static float f = 0.0f;
	ImGui::TextUnformatted("IMG!");
	ImGui::TextUnformatted("BIG KAHUNA");

	ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

	ImGui::Text("Camera");

	ImGui::SetNextWindowSize(ImVec2(200, 200), 2);
	ImGui::Begin("Example settings");
	ImGui::Checkbox("Render models", &uiSettings.displayModels);
	ImGui::Checkbox("Display logos", &uiSettings.displayLogos);
	ImGui::Checkbox("Display background", &uiSettings.displayBackground);
	ImGui::Checkbox("Animate light", &uiSettings.animateLight);
	ImGui::SliderFloat("Light speed", &uiSettings.lightSpeed, 0.1f, 1.0f);
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(650, 20), 2);
	ImGui::ShowDemoWindow();

	// Render to generate draw buffers
	ImGui::Render();

	{
		ImDrawData* imDrawData = ImGui::GetDrawData();

		// Note: Alignment is done inside buffer creation
		vk::DeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		vk::DeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
			return;
		}

		// Update buffers only if vertex or index count has been changed compared to current buffer size

		// Vertex buffer
		if ((!vertexBuffer) || (vertexCount != imDrawData->TotalVtxCount)) {
			if (vertexData)
			{
				framework->device.unmapMemory(vertexMemory);
				vertexData = nullptr;
			}

			if (vertexBuffer)
			{
				framework->device.destroyBuffer(vertexBuffer, nullptr);
			}
			if (vertexMemory)
			{
				framework->device.freeMemory(vertexMemory, nullptr);
			}

			vk::BufferCreateInfo bufferCreateInfo = { vk::BufferCreateFlags(), vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive, 0, nullptr };
			PSIM_ASSERT(framework->device.createBuffer(&bufferCreateInfo, nullptr, &vertexBuffer) == vk::Result::eSuccess, "");

			// Create the memory backing up the buffer handle
			vk::MemoryRequirements memReqs;
			vk::MemoryAllocateInfo memAlloc = { 0, 0 };
			framework->device.getBufferMemoryRequirements(vertexBuffer, &memReqs);
			memAlloc.allocationSize = memReqs.size;
			// Find a memory type index that fits the properties of the buffer
			
			memAlloc.memoryTypeIndex = framework->findMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
			PSIM_ASSERT(framework->device.allocateMemory(&memAlloc, nullptr, &vertexMemory) == vk::Result::eSuccess, "");

			void* data = nullptr;
			// If a pointer to the buffer data has been passed, map the buffer and copy over the data
			if (vertexData != nullptr)
			{
				PSIM_ASSERT(framework->device.mapMemory(vertexMemory, vk::DeviceSize(), vertexSize, vk::MemoryMapFlags(), &vertexData) == vk::Result::eSuccess, "");

				memcpy(vertexData, data, vertexSize);
				vk::MappedMemoryRange mappedRange = { vertexMemory, vk::DeviceSize(), vertexSize};
				PSIM_ASSERT(framework->device.flushMappedMemoryRanges(1, &mappedRange) == vk::Result::eSuccess, "");

				if (vertexData)
				{
					framework->device.unmapMemory(vertexMemory);
					vertexData = nullptr;
				}
			}

			// Initialize a default descriptor that covers the whole buffer size
			vertexDescriptor.offset = 0;
			vertexDescriptor.buffer = vertexBuffer;
			vertexDescriptor.range = vertexSize;

			// Attach the memory to the buffer object
			PSIM_ASSERT(framework->device.bindBufferMemory(vertexBuffer, vertexMemory, 0) == vk::Result::eSuccess, "");

			vertexCount = imDrawData->TotalVtxCount;
			framework->device.mapMemory(vertexMemory, vk::DeviceSize(), vertexSize, vk::MemoryMapFlags(), &vertexData);
		}

		// Index buffer
		if ((!indexBuffer) || (indexCount < imDrawData->TotalIdxCount)) {
			if (indexData)
			{
				framework->device.unmapMemory(indexMemory);
				indexData = nullptr;
			}

			if (indexBuffer)
			{
				framework->device.destroyBuffer(indexBuffer, nullptr);
			}
			if (indexMemory)
			{
				framework->device.freeMemory(indexMemory, nullptr);
			}
			
			vk::BufferCreateInfo bufferCreateInfo = { vk::BufferCreateFlags(), indexBufferSize, vk::BufferUsageFlagBits::eIndexBuffer, vk::SharingMode::eExclusive, 0, nullptr };
			PSIM_ASSERT(framework->device.createBuffer(&bufferCreateInfo, nullptr, &indexBuffer) == vk::Result::eSuccess, "");

			// Create the memory backing up the buffer handle
			vk::MemoryRequirements memReqs;
			vk::MemoryAllocateInfo memAlloc = { 0, 0 };
			framework->device.getBufferMemoryRequirements(indexBuffer, &memReqs);
			memAlloc.allocationSize = memReqs.size;
			// Find a memory type index that fits the properties of the buffer

			memAlloc.memoryTypeIndex = framework->findMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
			PSIM_ASSERT(framework->device.allocateMemory(&memAlloc, nullptr, &indexMemory) == vk::Result::eSuccess, "");

			void* data = nullptr;
			// If a pointer to the buffer data has been passed, map the buffer and copy over the data
			if (indexData != nullptr)
			{
				PSIM_ASSERT(framework->device.mapMemory(indexMemory, vk::DeviceSize(), indexSize, vk::MemoryMapFlags(), &indexData) == vk::Result::eSuccess, "");

				memcpy(indexData, data, indexSize);
				vk::MappedMemoryRange mappedRange = { indexMemory, vk::DeviceSize(), indexSize };
				PSIM_ASSERT(framework->device.flushMappedMemoryRanges(1, &mappedRange) == vk::Result::eSuccess, "");

				if (indexData)
				{
					framework->device.unmapMemory(indexMemory);
					indexData = nullptr;
				}
			}

			// Initialize a default descriptor that covers the whole buffer size
			indexDescriptor.offset = 0;
			indexDescriptor.buffer = indexBuffer;
			indexDescriptor.range = indexSize;

			// Attach the memory to the buffer object
			PSIM_ASSERT(framework->device.bindBufferMemory(indexBuffer, indexMemory, 0) == vk::Result::eSuccess, ""); 
			
			indexCount = imDrawData->TotalIdxCount;
			framework->device.mapMemory(indexMemory, vk::DeviceSize(), indexSize, vk::MemoryMapFlags(), &indexData);
	}

	// Upload data
	ImDrawVert* vtxDst = (ImDrawVert*)vertexData;
	ImDrawIdx* idxDst = (ImDrawIdx*)indexData;

	for (int n = 0; n < imDrawData->CmdListsCount; n++) {
		const ImDrawList* cmd_list = imDrawData->CmdLists[n];
		memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmd_list->VtxBuffer.Size;
		idxDst += cmd_list->IdxBuffer.Size;
	}

	// Flush to make writes visible to GPU
	vk::MappedMemoryRange mappedRange = { vertexMemory, vk::DeviceSize(), vertexSize };
	PSIM_ASSERT(framework->device.flushMappedMemoryRanges(1, &mappedRange) == vk::Result::eSuccess, "");
	mappedRange = { indexMemory, vk::DeviceSize(), indexSize };
	PSIM_ASSERT(framework->device.flushMappedMemoryRanges(1, &mappedRange) == vk::Result::eSuccess, "");
	}

	vk::Result err;
	{
		err = framework->device.resetCommandPool(imGuiCommandPool, {});
		check_vk_result((VkResult)err);
		vk::CommandBufferBeginInfo info = { vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr};
		err = imGuiCommandBuffers[framework->currentFrame].begin(&info);
		check_vk_result((VkResult)err);
	}
	{
		vk::Rect2D rectangle = { {0, 0}, vk::Extent2D {static_cast<uint32_t>(imGuiWidth), static_cast<uint32_t>(imGuiHeight)} };
		vk::RenderPassBeginInfo info = { framework->renderPass, framework->swapChainFramebuffers[framework->currentFrame], rectangle, 2, &(framework->clearValues[0]) };
		imGuiCommandBuffers[framework->currentFrame].beginRenderPass(&info, vk::SubpassContents::eInline);
	}

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)imGuiCommandBuffers[framework->currentFrame]);

	imGuiCommandBuffers[framework->currentFrame].endRenderPass();
	{
		vk::Semaphore waitSemaphores[] = { framework->imageAvailableSemaphores[framework->currentFrame] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::Semaphore signalSemaphores[] = { framework->renderFinishedSemaphores[framework->currentFrame] };
		const vk::CommandBuffer& tempBuffer = imGuiCommandBuffers[framework->currentFrame];
		vk::SubmitInfo info = { 1, waitSemaphores, waitStages, 1, &tempBuffer, 1, signalSemaphores };

		err = imGuiCommandBuffers[framework->currentFrame].end();
		check_vk_result((VkResult)err);
		err = framework->graphicsQueue.submit(1, &info, framework->inFlightFences[framework->currentFrame]);
		check_vk_result((VkResult)err);
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void VulkanImGui::initResources(vk::RenderPass renderPass, vk::Queue copyQueue)
{
	ImGuiIO& io = ImGui::GetIO();

	createImGuiCommandBuffers();

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

	framework->createImage(texWidth, texHeight, 1, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, fontImage, fontMemory);
	fontView = framework->createImageView(fontImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, 1);

	// Staging buffers for font data upload
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	framework->createBuffer(uploadSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data;
	framework->device.mapMemory(stagingBufferMemory, 0, uploadSize, vk::MemoryMapFlags(), &data);
	memcpy(data, fontData, uploadSize);
	framework->device.unmapMemory(stagingBufferMemory);

	// Copy buffer data to font image
	vk::CommandBufferAllocateInfo allocInfo = { imGuiCommandPool, vk::CommandBufferLevel::ePrimary, 1 };
	vk::CommandBuffer copyCmd;
	PSIM_ASSERT(framework->device.allocateCommandBuffers(&allocInfo, &copyCmd) == vk::Result::eSuccess, "");

	vk::CommandBufferBeginInfo cmdBufInfo = {};
	PSIM_ASSERT(copyCmd.begin(&cmdBufInfo) == vk::Result::eSuccess, "");

	// Prepare for transfer
	vk::ImageMemoryBarrier imageMemBarrier = { vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, fontImage, vk::ImageSubresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } }; 
	copyCmd.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemBarrier);
	
	// Copy
	vk::DeviceSize sizeNone = 0;
	uint32_t valueZero = 0;
	vk::BufferImageCopy bufferCopyRegion = { static_cast<vk::DeviceSize>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0), vk::ImageSubresourceLayers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 }, { 0, 0, 0 }, { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), static_cast<uint32_t>(1)} };

	copyCmd.copyBufferToImage(stagingBuffer, fontImage, vk::ImageLayout::eTransferDstOptimal, 1, &bufferCopyRegion);

	// Prepare for shader read
	imageMemBarrier = { vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, fontImage, vk::ImageSubresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };
	copyCmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemBarrier);

	if ((VkCommandBuffer)copyCmd != VK_NULL_HANDLE)
	{
		PSIM_ASSERT(copyCmd.end() == vk::Result::eSuccess, "");

		vk::SubmitInfo submitInfo = { 0, nullptr, nullptr, 0, &copyCmd, 0, nullptr };

		// Create fence to ensure that the command buffer has finished executing
		vk::FenceCreateInfo fenceInfo = {};
		vk::Fence fence;
		PSIM_ASSERT(framework->device.createFence(&fenceInfo, nullptr, &fence) == vk::Result::eSuccess, "");
		// Submit to the queue
		PSIM_ASSERT(copyQueue.submit(1, &submitInfo, fence) == vk::Result::eSuccess, "");
		// Wait for the fence to signal that command buffer has finished executing
		PSIM_ASSERT(framework->device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX) == vk::Result::eSuccess, "");
		framework->device.destroyFence(fence, nullptr);
		if (free)
		{
			framework->device.freeCommandBuffers(imGuiCommandPool, 1, &copyCmd);
		}
	}

	framework->device.destroyBuffer(stagingBuffer, nullptr);
	framework->device.freeMemory(stagingBufferMemory, nullptr);

	// Font texture Sampler
	vk::SamplerCreateInfo samplerInfo = { vk::SamplerCreateFlags(), vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, 0, 0, 0.0f, 0, vk::CompareOp::eNever, 0, 0, vk::BorderColor::eFloatOpaqueWhite, 0 };
	PSIM_ASSERT(framework->device.createSampler(&samplerInfo, nullptr, &sampler) == vk::Result::eSuccess, "");

	// Descriptor pool
	vk::DescriptorPoolSize poolSizes[] = { { vk::DescriptorType::eCombinedImageSampler, 1 } };

	vk::DescriptorPoolCreateInfo descriptorPoolInfo = { {}, 2, static_cast<uint32_t>(1), poolSizes };
	PSIM_ASSERT(framework->device.createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptorPool) == vk::Result::eSuccess, "");

	// Descriptor set layout
	vk::DescriptorSetLayoutBinding setLayoutBindings[] = { { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr } };

	vk::DescriptorSetLayoutCreateInfo descriptorLayout = { {}, 1, setLayoutBindings };
	PSIM_ASSERT(framework->device.createDescriptorSetLayout(&descriptorLayout, nullptr, &descriptorSetLayout) == vk::Result::eSuccess, "");

	// Descriptor set
	vk::DescriptorSetAllocateInfo descAllocInfo = { descriptorPool, 1, &descriptorSetLayout };
	PSIM_ASSERT(framework->device.allocateDescriptorSets(&descAllocInfo, &descriptorSet) == vk::Result::eSuccess, "");

	vk::DescriptorImageInfo fontDescriptor = { sampler, fontView, vk::ImageLayout::eShaderReadOnlyOptimal };

	vk::WriteDescriptorSet writeDescriptorSets[] = { { descriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &fontDescriptor, nullptr, nullptr } };

	framework->device.updateDescriptorSets(static_cast<uint32_t>(1), writeDescriptorSets, 0, nullptr);

	// Pipeline layout
	// Push constants for UI rendering parameters
	vk::PushConstantRange pushConstantRange = { vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstBlock) };
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = { vk::PipelineLayoutCreateFlags(), 1, &descriptorSetLayout, 1, &pushConstantRange };
	PSIM_ASSERT(framework->device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &pipelineLayout) == vk::Result::eSuccess, "");

	// Setup graphics pipeline for UI rendering
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = { {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE };

	vk::PipelineRasterizationStateCreateInfo rasterizationState = { {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f };

	// Enable blending
	vk::PipelineColorBlendAttachmentState blendAttachmentState{ VK_TRUE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

	vk::PipelineColorBlendStateCreateInfo colorBlendState = { {}, 0, vk::LogicOp::eClear, 1, &blendAttachmentState };

	vk::PipelineDepthStencilStateCreateInfo depthStencilState = { {}, VK_FALSE, VK_FALSE, vk::CompareOp::eLessOrEqual, 0, 0, vk::StencilOpState(), vk::StencilOpState(), 0.0f, 0.0f };
	depthStencilState.front = depthStencilState.back;
	depthStencilState.back.compareOp = vk::CompareOp::eAlways;

	vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, nullptr, 1, nullptr };

	vk::PipelineMultisampleStateCreateInfo multisampleState = { {}, framework->msaaSamples, true, 0.2f, nullptr, false, false };

	vk::DynamicState dynamicStateEnables[] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamicState = { {}, 2, dynamicStateEnables };

	std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{};

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pDynamicState = &dynamicState;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	pipelineCreateInfo.pStages = shaderStages.data();

	// Vertex bindings an attributes based on ImGui vertex definition
	auto bindingDescription = VulkanFrameWork::Vertex::getBindingDescription();
	auto attributeDescriptions = VulkanFrameWork::Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputState = { {}, static_cast<uint32_t>(1), &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data() };

	pipelineCreateInfo.pVertexInputState = &vertexInputState;

	auto vertShaderCode = framework->readFileByteCode("assets/shaders/ImGuiVert.spv");
	auto fragShaderCode = framework->readFileByteCode("assets/shaders/ImGuiFrag.spv");

	vk::ShaderModule vertShaderModule = framework->createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = framework->createShaderModule(fragShaderCode);

	//Vertex SHader Stage Info
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main" };

	//Fragment Shader Stage Info
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main" };

	shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

	PSIM_ASSERT(framework->device.createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline) == vk::Result::eSuccess, "");
}

void VulkanImGui::createImGuiCommandBuffers() {
	PSIM_PROFILE_FUNCTION();

	//allocate command buffers
	vk::CommandBufferAllocateInfo allocInfo = { imGuiCommandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)imGuiCommandBuffers.size() };

	PSIM_ASSERT(framework->device.allocateCommandBuffers(&allocInfo, imGuiCommandBuffers.data()) == vk::Result::eSuccess, "Failed to allocate command buffers!");
}