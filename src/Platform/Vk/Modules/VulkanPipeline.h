#pragma once
#include "vulkan/vulkan.hpp"
#include "Platform/Vk/Modules/VulkanShader.h"
	

class VulkanPipeline
{
public:
	//create the linked shaders
	VulkanPipeline(Ref<VulkanLinkedShader> shader); //will need to add things like 
	VulkanPipeline(const std::string& name, Ref<VulkanLinkedShader> shader);
	~VulkanPipeline();

	vk::PipelineLayout getPipelineLayout() { return m_pipelineLayout; }
	vk::Pipeline getPipeline() { return m_pipeline; }

	//returns nameof the linked shader
	 const std::string& getName() const  { return m_Name; }

private:
	std::string m_Name;
	Ref<VulkanLinkedShader> m_shader;
	vk::PipelineLayout m_pipelineLayout;
	vk::Pipeline m_pipeline;
};

class VulkanPipelineLibrary
{
public:
	static VulkanPipelineLibrary *getPipelineLibrary() {
		if (!m_pipelineLib)
			m_pipelineLib = new VulkanPipelineLibrary;
		return m_pipelineLib;
	}

	~VulkanPipelineLibrary() {}

	//add a completed Linked Shader from another Library
	void add(const std::string& name, Ref<VulkanPipeline> pipeline);

	//add a new shader grouping, name created automatically
	 void load(Ref<VulkanLinkedShader> shader) ;

	//add a new shader grouping with a name
	 void load(const std::string& name, Ref<VulkanLinkedShader> shader) ;

	//returns the specified linkedShader
	 Ref<VulkanPipeline> get(const std::string& name);

	//checks if a shader exists
	 bool exists(const std::string& name) const ;

private:
	std::unordered_map<std::string, Ref<VulkanPipeline>> m_VulkanPipelines;
	static VulkanPipelineLibrary *m_pipelineLib;
};




class VulkanRenderPass
{
public:
	//create the linked shaders
	VulkanRenderPass();
	~VulkanRenderPass();
	
	void recreate();

	vk::RenderPass getrenderPass() { return m_renderPass; }

private:
	vk::RenderPass m_renderPass;
};

class VulkanRenderPassLibrary
{
public:
	static VulkanRenderPassLibrary *getRenderPassLibrary() {
		if (!m_renderPassLib)
		{
			m_renderPassLib = new VulkanRenderPassLibrary;
		}
		return m_renderPassLib;
	}

	~VulkanRenderPassLibrary() {}

	//add a completed Linked Shader from another Library
	void add(Ref<VulkanRenderPass> renderPass);

	//add a new shader grouping, name created automatically
	Ref<VulkanRenderPass> load();

	//returns the specified linkedShader
	Ref<VulkanRenderPass> get(int ID);

	//checks if a shader exists
	bool exists(int ID) const;

	void destroy(int ID);

private:
	std::unordered_map<int, Ref<VulkanRenderPass>> m_VulkanRenderPasses;
	static VulkanRenderPassLibrary *m_renderPassLib;
};