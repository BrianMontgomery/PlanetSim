#include "PSIMPCH.h"
#include "VulkanPipeline.h"

#include "Platform/Vk/Modules/VulkanShader.h"
#include "Platform/Vk/Modules/VulkanBuffer.h"
#include "Platform/Vk/Modules/VulkanDepthBuffer.h"

VulkanPipeline::VulkanPipeline()
{
}


VulkanPipeline::~VulkanPipeline()
{
}

vk::RenderPass VulkanPipeline::createRenderPass(vk::Format swapchainImageFormat, vk::PhysicalDevice& physicalDevice, vk::Device& device)
{
	//color attachment
	vk::AttachmentDescription colorAttachment = { {}, swapchainImageFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR };

	//depth attachment
	VulkanDepthBuffer depthMaker;
	vk::AttachmentDescription depthAttachment = { {}, depthMaker.findDepthFormat(physicalDevice), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal };

	//color attachment reference
	vk::AttachmentReference colorAttachmentRef = { 0, vk::ImageLayout::eColorAttachmentOptimal };

	//depth attachment reference
	vk::AttachmentReference depthAttachmentRef = { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

	//subpass
	vk::SubpassDescription subpass = { {}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef, nullptr, &depthAttachmentRef, 0, nullptr };

	//dependencies
	vk::SubpassDependency dependency = { VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
	vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, {} };

	//create render pass
	std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	vk::RenderPassCreateInfo renderPassInfo = { {}, static_cast<uint32_t>(attachments.size()), attachments.data(), 1, &subpass, 1, &dependency };

	vk::RenderPass renderPass;
	PSIM_ASSERT(device.createRenderPass(&renderPassInfo, nullptr, &renderPass) == vk::Result::eSuccess, "Failed to create render pass!");
	return renderPass;
}

vk::Pipeline VulkanPipeline::createGraphicsPipeline(vk::Extent2D& swapchainExtent, vk::Device& device, vk::RenderPass& renderPass, vk::DescriptorSetLayout& descriptorSetlayout)
{
	VulkanShader shaderMaker;
	vk::ShaderModule vertShaderModule = shaderMaker.createShaderModule("src/Platform/Vk/Shaders/TriangleShaderVert.spv", device);
	vk::ShaderModule fragShaderModule = shaderMaker.createShaderModule("src/Platform/Vk/Shaders/TriangleShaderFrag.spv", device);

	//Vertex SHader Stage Info
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main" };

	//Fragment Shader Stage Info
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Vertex Input Stage Info
	auto bindingDescription = VulkanBuffer::Vertex::getBindingDescription();
	auto attributeDescriptions = VulkanBuffer::Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, 1, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data() };

	//Input Assembly Stage Info
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, false };

	//Viewport info
	vk::Viewport viewport = { 0.0f, 0.0f, (float)swapchainExtent.width, (float)swapchainExtent.height, 0.0f, 1.0f };

	//Scissor Info
	vk::Rect2D scissor = { { 0, 0 }, swapchainExtent };

	//Viewport State Info
	vk::PipelineViewportStateCreateInfo viewportState = { {}, 1, &viewport, 1, &scissor };

	//Rasterizer Info
	vk::PipelineRasterizationStateCreateInfo rasterizer = { {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, false, 0.0f, 0.0f, 0.0f, 1.0f };

	//MultiSampling Info
	vk::PipelineMultisampleStateCreateInfo multisampling = { {}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false };

	vk::PipelineDepthStencilStateCreateInfo depthStencil = { {}, true, true, vk::CompareOp::eLess, false, false };

	//Color Blending Info
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = { true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne,
	vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

	//Color Blend State Info
	vk::PipelineColorBlendStateCreateInfo colorBlending = { {}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment };

	//Dynamic State Info
	vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

	vk::PipelineDynamicStateCreateInfo dynamicState = { {}, 2, dynamicStates };

	//Pipeline Layout Info
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &descriptorSetlayout };

	//create pipeline layout
	PSIM_ASSERT(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) == vk::Result::eSuccess, "Failed to create pipeline layout!");

	//creating the pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil,
	&colorBlending, nullptr, pipelineLayout, renderPass, 0 };

	vk::Pipeline graphicsPipeline;
	PSIM_ASSERT(device.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &graphicsPipeline) == vk::Result::eSuccess, "Failed to create graphics pipeline!");
	PSIM_CORE_INFO("Created Graphics Pipeline");

	//cleanup
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

	return graphicsPipeline;
}