#pragma once
#include "vulkan/vulkan.hpp"
#include "Platform/Vk/Modules/VulkanShader.h"
	

class VulkanPipeline
{
public:
	//create the linked shaders
	VulkanPipeline(const Ref<VulkanLinkedShader> shader); //will need to add things like 
	VulkanPipeline(const std::string& name, const Ref<VulkanLinkedShader> shader);
	~VulkanPipeline();

	vk::RenderPass createRenderPass();

	vk::PipelineLayout getPipelineLayout() { return m_pipelineLayout; }
	vk::Pipeline getPipeline() { return m_pipeline; }

	//returns nameof the linked shader
	 const std::string& getName() const  { return m_Name; }

private:
	std::string m_Name;
	const Ref<VulkanLinkedShader> m_shader;
	vk::PipelineLayout m_pipelineLayout;
	vk::Pipeline m_pipeline;
};

class VulkanPipelineLibrary
{
public:
	//add a completed Linked Shader from another Library
	void add(const std::string& name, const Ref<VulkanPipeline> pipeline);

	//add a new shader grouping, name created automatically
	 Ref<VulkanPipeline> load(const Ref<VulkanLinkedShader> shader) ;

	//add a new shader grouping with a name
	 Ref<VulkanPipeline> load(const std::string& name, const Ref<VulkanLinkedShader> shader) ;

	//returns the specified linkedShader
	 Ref<VulkanPipeline> get(const std::string& name) ;

	//checks if a shader exists
	 bool exists(const std::string& name) const ;
private:
	std::unordered_map<std::string, Ref<VulkanPipeline>> m_VulkanPipelines;
};