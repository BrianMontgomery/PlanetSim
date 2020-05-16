#include "PSIMPCH.h"
#include "VulkanPipeline.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include "Platform/Vk/Modules/VulkanShader.h"
#include "Platform/Vk/Modules/VulkanBuffer.h"
#include "Platform/Vk/Modules/VulkanDepthBuffer.h"

VulkanPipeline::VulkanPipeline(const Ref<VulkanLinkedShader> shader)
{
	
}

VulkanPipeline::VulkanPipeline(const std::string& name, const Ref<VulkanLinkedShader> shader)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//Vertex SHader Stage Info
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = { {}, vk::ShaderStageFlagBits::eVertex, shader->getVertexShader()->getModule(), "main" };

	//Fragment Shader Stage Info
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = { {}, vk::ShaderStageFlagBits::eFragment, shader->getFragmentShader()->getModule(), "main" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Vertex Input Stage Info
	auto bindingDescription = VulkanBuffer::Vertex::getBindingDescription();
	auto attributeDescriptions = VulkanBuffer::Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = { {}, 1, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data() };

	//Input Assembly Stage Info
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {}, vk::PrimitiveTopology::eTriangleList, false };

	//Viewport info
	vk::Viewport viewport = { 0.0f, 0.0f, (float)framework->swapchainExtent.width, (float)framework->swapchainExtent.height, 0.0f, 1.0f };

	//Scissor Info
	vk::Rect2D scissor = { { 0, 0 }, framework->swapchainExtent };

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
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = { {}, 1, &framework->descriptorSetLayout };

	//create pipeline layout
	PSIM_ASSERT(framework->device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_pipelineLayout) == vk::Result::eSuccess, "Failed to create pipeline layout!");

	//creating the pipeline
	VulkanRenderPassLibrary * renderPassLib = VulkanRenderPassLibrary::getRenderPassLibrary();
	vk::GraphicsPipelineCreateInfo pipelineInfo = { {}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, &depthStencil,
	&colorBlending, nullptr, m_pipelineLayout, renderPassLib->get(0)->getrenderPass(), 0 };

	PSIM_ASSERT(framework->device.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &m_pipeline) == vk::Result::eSuccess, "Failed to create graphics pipeline!");
	PSIM_CORE_INFO("Created Graphics Pipeline");

	m_Name = name;
	*m_shader = *shader;
}

VulkanPipeline::~VulkanPipeline()
{

}

void VulkanPipelineLibrary::add(const std::string& name, Ref<VulkanPipeline> pipeline)
{
	PSIM_ASSERT(!exists(name), "This Pipeline: {0} already exists!", name);
	m_VulkanPipelines[name] = pipeline;
}

void VulkanPipelineLibrary::load(Ref<VulkanLinkedShader> shader)
{
	PSIM_PROFILE_FUNCTION();

	auto pipeline = CreateRef<VulkanPipeline>(shader);
	add(pipeline->getName(), pipeline);
	return;
}

void VulkanPipelineLibrary::load(const std::string& name, Ref<VulkanLinkedShader> shader)
{
	PSIM_PROFILE_FUNCTION();

	auto pipeline = CreateRef<VulkanPipeline>(shader);
	add(pipeline->getName(), pipeline);
	return;
}

Ref<VulkanPipeline> VulkanPipelineLibrary::get(const std::string& name)
{
	PSIM_ASSERT(exists(name), "Pipeline not found!");
	return m_VulkanPipelines[name];
}

bool VulkanPipelineLibrary::exists(const std::string& name) const
{
	return m_VulkanPipelines.find(name) != m_VulkanPipelines.end();
}



VulkanRenderPass::VulkanRenderPass() 
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//color attachment - contains format - multisampling - whether to store framebuffer data or not - image layout on way in and out
	vk::AttachmentDescription colorAttachment = { {}, framework->swapchainImageFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR };

	//depth attachment - contains format - multisampling - whether to store framebuffer data or not - image layout on way in and out
	VulkanDepthBuffer depthMaker;
	vk::AttachmentDescription depthAttachment = { {}, depthMaker.findDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
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

	PSIM_ASSERT(framework->device.createRenderPass(&renderPassInfo, nullptr, &m_renderPass) == vk::Result::eSuccess, "Failed to create render pass!");
}

VulkanRenderPass::~VulkanRenderPass()
{
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	framework->device.destroyRenderPass(m_renderPass, nullptr);
}

//to be used when the swapchain changes due to resize
void VulkanRenderPass::recreate()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	//color attachment - contains format - multisampling - whether to store framebuffer data or not - image layout on way in and out
	vk::AttachmentDescription colorAttachment = { {}, framework->swapchainImageFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
	vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR };

	//depth attachment - contains format - multisampling - whether to store framebuffer data or not - image layout on way in and out
	VulkanDepthBuffer depthMaker;
	vk::AttachmentDescription depthAttachment = { {}, depthMaker.findDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
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

	PSIM_ASSERT(framework->device.createRenderPass(&renderPassInfo, nullptr, &m_renderPass) == vk::Result::eSuccess, "Failed to create render pass!");
}

void VulkanRenderPassLibrary::add(Ref<VulkanRenderPass> renderPass)
{
	for (int i = 0; i < m_VulkanRenderPasses.size(); i++)
	{
		PSIM_ASSERT(!exists(i), "This renderPass: {0} already exists!", i);
		if (renderPass == m_VulkanRenderPasses[i]) 
		{
			return;
		}
	}

	for (int i = 0; i <= m_VulkanRenderPasses.size(); i++)
	{
		if (!exists(i))
		{
			m_VulkanRenderPasses[i] = renderPass;
			return;
		}
	}

	PSIM_ASSERT(0, "Could not add renderPass to the Render Pass Library!");
}

//add a new shader grouping, name created automatically
Ref<VulkanRenderPass> VulkanRenderPassLibrary::load()
{
	PSIM_PROFILE_FUNCTION();

	auto renderPass = CreateRef<VulkanRenderPass>();
	add(renderPass);
	return renderPass;
}

//returns the specified linkedShader
Ref<VulkanRenderPass> VulkanRenderPassLibrary::get(int ID)
{
	PSIM_ASSERT(exists(ID), "RenderPass not found!");
	return m_VulkanRenderPasses[ID];
}

//checks if a shader exists
bool VulkanRenderPassLibrary::exists(int ID) const
{
	return m_VulkanRenderPasses.find(ID) != m_VulkanRenderPasses.end();
}

void VulkanRenderPassLibrary::destroy(int ID)
{
	PSIM_ASSERT(exists(ID), "RenderPass not found!");
	m_VulkanRenderPasses.erase(ID);
	return;
}