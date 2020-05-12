#pragma once
#include "vulkan/vulkan.hpp"
#include "PSIM/Graphics/Shader.h"

class VulkanShader
{
public:
	VulkanShader(const std::string& ShaderPath);
	VulkanShader(const std::string& name, const std::string& shaderPath);
	~VulkanShader();

	VulkanShader operator= (VulkanShader shader) {
		this->m_Path = shader.m_Path;
		this->m_Name = shader.m_Name;
		this->m_Type = shader.m_Type;
		this->m_Module = shader.m_Module;

		return *this;
	}

	//return funcs for important values
	const std::string& getName() const { return m_Name; }
	const std::string getType() const;
	const vk::ShaderModule getModule() const { return m_Module; }

	//create shader modules for each
	void compile();

private:
	//get the contents of the file
	static std::vector<char> readFileByteCode(const std::string& filename);

	//preliminary analysis functions of the name
	const std::string extractExtension();
	const std::string extractName();
	const vk::ShaderStageFlagBits extractType();

	std::string m_Path;
	std::string m_Name;
	vk::ShaderStageFlagBits m_Type;
	vk::ShaderModule m_Module;
};

class VulkanShaderLibrary
{
public:
	//add a shader from another lib
	void add(const std::string& name, const Ref<VulkanShader>& shader);

	//load individual shader component
	Ref<VulkanShader> load(const std::string& filepath);
	Ref<VulkanShader> load(const std::string& name, const std::string& filepath);

	//return each individual shader
	Ref<VulkanShader> get(const std::string& name);
	bool exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<VulkanShader>> m_Shaders;
};

class VulkanLinkedShader : public LinkedShader
{
public:
	//create the linked shaders
	VulkanLinkedShader(const Ref<VulkanShader> vertexShader, const Ref<VulkanShader> fragmentShader);
	VulkanLinkedShader(const std::string& name, const Ref<VulkanShader> vertexShader, const Ref<VulkanShader> fragmentShader);
	virtual ~VulkanLinkedShader() override;

	VulkanLinkedShader operator= (VulkanLinkedShader shader) {
		this->m_Name = shader.m_Name;
		*this->m_VertexShader = *shader.m_VertexShader;
		*this->m_FragmentShader = *shader.m_FragmentShader;

		return *this;
	}

	//create a shader module for Vulkan
	virtual void bind() override;

	//delete Vulkan Shader module
	virtual void unbind() override;

	//user controlled variables
	virtual void setInt(const std::string& name, int value) override;
	virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
	virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
	virtual void setMat4(const std::string& name, const glm::mat4& value) override;

	//returns nameof the linked shader
	virtual const std::string& getName() const override { return m_Name; }
	const Ref<VulkanShader> getVertexShader() const { return m_VertexShader; }
	const Ref<VulkanShader> getFragmentShader() const { return m_FragmentShader; }

private:
	std::string m_Name;
	const Ref<VulkanShader> m_VertexShader;
	const Ref<VulkanShader> m_FragmentShader;
};

class VulkanLinkedShaderLibrary : public ShaderLibrary
{
public:
	//add a completed Linked Shader from another Library
	void add(const std::string& name, const Ref<VulkanLinkedShader> linkedShader);

	//add a new shader grouping, name created automatically
	virtual Ref<LinkedShader> load(const std::string& vertexPath, const std::string& fragmentPath) override;

	//add a new shader grouping with a name
	virtual Ref<LinkedShader> load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) override;

	//returns the specified linkedShader
	virtual Ref<LinkedShader> get(const std::string& name) override;

	//checks if a shader exists
	virtual bool exists(const std::string& name) const override;
private:
	std::unordered_map<std::string, Ref<VulkanLinkedShader>> m_LinkedShaders;
};