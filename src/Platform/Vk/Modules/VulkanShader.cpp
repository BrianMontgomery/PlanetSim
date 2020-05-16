#include "PSIMPCH.h"
#include "VulkanShader.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"


VulkanShader::VulkanShader(const std::string& shaderPath)
	: m_Path(shaderPath)
{
	std::string m_Name = extractName();
	 m_Type = extractType();
}

VulkanShader::VulkanShader(const std::string& name, const std::string& shaderPath)
	: m_Name(name), m_Path(shaderPath)
{
	std::string m_Name = extractName();
	m_Type = extractType();
}

VulkanShader::~VulkanShader()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	vkDestroyShaderModule(framework->device, this->m_Module, nullptr);
}

const std::string VulkanShader::getType() const 
{
	if (m_Type == vk::ShaderStageFlagBits::eVertex)
	{
		return "vertex";
	}
	else if (m_Type == vk::ShaderStageFlagBits::eFragment)
	{
		return "fragment";
	}

	PSIM_ASSERT(false, "Type not found!");
	return nullptr;
}

std::vector<char> VulkanShader::readFileByteCode(const std::string& filename)
{
	PSIM_PROFILE_FUNCTION();
	//open file
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	PSIM_ASSERT(file.is_open(), "Failed to open file!");

	//allocate buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//read the file
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	//close the file
	file.close();

	return buffer;
}

void VulkanShader::compile()
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();

	const std::vector<char>& code = readFileByteCode(m_Path);
	//shader module  info
	vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

	//create it
	PSIM_ASSERT(framework->device.createShaderModule(&createInfo, nullptr, &m_Module) == vk::Result::eSuccess, "Failed to create shader module!");
}

const std::string VulkanShader::extractExtension()
{
	PSIM_PROFILE_FUNCTION();

	std::string result;

	size_t extensionStart = m_Path.find_last_of('.');
	std::cout << result << std::endl;

	return result;
}

const std::string VulkanShader::extractName()
{
	//extract name from filepath
	auto extension = extractExtension();

	auto lastSlash = m_Path.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = m_Path.rfind('.');
	auto count = lastDot == std::string::npos ? m_Path.size() - lastSlash : lastDot - lastSlash;

	if (extension == ".spv")
	{
		return (m_Path.substr(lastSlash, count));
	}
	else if (extension == ".vert") {
		std::string name = m_Path.substr(lastSlash, count);
		return (name + "Vert");
	}
	else if (extension == ".frag") {
		std::string name = m_Path.substr(lastSlash, count);
		return (name + "Frag");
	}

	return nullptr;
}

const vk::ShaderStageFlagBits VulkanShader::extractType()
{
	std::string extension = extractExtension();

	if (extension == ".spv") {
		//extract name from filepath
		auto lastDot = m_Path.rfind('.');
		auto type = (m_Path.substr(lastDot - 4, 4));

		if (type == "vert" || type == "Vert")
		{
			return vk::ShaderStageFlagBits::eVertex;
		}
		else if (type == "frag" || type == "Frag") {
			return vk::ShaderStageFlagBits::eFragment;
		}
	}
	else if (extension == ".vert") {
		return vk::ShaderStageFlagBits::eVertex;
	}
	else if (extension == ".frag") {
		return vk::ShaderStageFlagBits::eFragment;
	}

	PSIM_ASSERT(false, "Type not found!");
	return vk::ShaderStageFlagBits::eAll;
}

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shader Library
//---------------------------------------------------------------------------------------------------------------------------------------

void VulkanShaderLibrary::add(const std::string& name, const Ref<VulkanShader>& shader)
{
	PSIM_ASSERT(!exists(name), "This Shader: {0} already exists!", name);
	m_Shaders[name] = shader;
}

Ref<VulkanShader> VulkanShaderLibrary::load(const std::string& filepath)
{
	PSIM_PROFILE_FUNCTION();

	auto shader = CreateRef<VulkanShader>(filepath);
	shader->compile();
	add(shader->getName(), shader);
	return shader;
}

Ref<VulkanShader> VulkanShaderLibrary::load(const std::string& name, const std::string& filepath)
{
	PSIM_PROFILE_FUNCTION();

	auto shader = CreateRef<VulkanShader>(filepath);
	shader->compile();
	add(name, shader);
	return shader;
}

Ref<VulkanShader> VulkanShaderLibrary::get(const std::string& name)
{
	PSIM_ASSERT(exists(name), "Shader not found!");
	return m_Shaders[name];
}


bool VulkanShaderLibrary::exists(const std::string& name) const
{
	return m_Shaders.find(name) != m_Shaders.end();
}

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shaders
//---------------------------------------------------------------------------------------------------------------------------------------

VulkanLinkedShader::VulkanLinkedShader(const Ref<VulkanShader> vertexShader, const Ref<VulkanShader> fragmentShader)
	: m_VertexShader(vertexShader), m_FragmentShader(fragmentShader)
{
	m_Name = vertexShader->getName().substr(0, (vertexShader->getName().size() - 4));
}

VulkanLinkedShader::VulkanLinkedShader(const std::string& name, const Ref<VulkanShader> vertexShader, const Ref<VulkanShader> fragmentShader)
	: m_VertexShader(vertexShader), m_FragmentShader(fragmentShader), m_Name(name)
{}

VulkanLinkedShader::~VulkanLinkedShader()
{}

void VulkanLinkedShader::bind()
{
	//create pipeline and bind it
}

void VulkanLinkedShader::unbind()
{

}

void VulkanLinkedShader::setInt(const std::string& name, int value)
{

}

void VulkanLinkedShader::setFloat3(const std::string& name, const glm::vec3& value)
{

}

void VulkanLinkedShader::setFloat4(const std::string& name, const glm::vec4& value)
{

}

void VulkanLinkedShader::setMat4(const std::string& name, const glm::mat4& value)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shaders
//---------------------------------------------------------------------------------------------------------------------------------------

void VulkanLinkedShaderLibrary::add(const std::string& name, const Ref<VulkanLinkedShader> linkedShader)
{
	PSIM_ASSERT(!exists(name), "This Linked Shader: {0} group already exists!", name);
	m_LinkedShaders[name] = linkedShader;
}

void VulkanLinkedShaderLibrary::load(const std::string& vertexPath, const std::string& fragmentPath)
{
	PSIM_PROFILE_FUNCTION();

	VulkanShaderLibrary shaderLibrary;
	//load vertex shader
	auto vertexShader = shaderLibrary.load(vertexPath);
	auto vertexShaderName = vertexShader->getName();

	//load fragment shader
	auto fragmentShader = shaderLibrary.load(fragmentPath);
	auto fragmentShaderName = fragmentShader->getName();

	//link shaders together
	auto linkedShader = CreateRef<VulkanLinkedShader>(shaderLibrary.get(vertexShaderName), shaderLibrary.get(fragmentShaderName));

	//add to the linked shader list
	add(linkedShader->getName(), linkedShader);
	return;
}

void VulkanLinkedShaderLibrary::load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
	PSIM_PROFILE_FUNCTION();

	//load vertex shader
	VulkanShaderLibrary shaderLibrary;
	auto vertexShader = shaderLibrary.load(vertexPath);
	auto vertexShaderName = vertexShader->getName();

	//load fragment shader
	auto fragmentShader = shaderLibrary.load(fragmentPath);
	auto fragmentShaderName = fragmentShader->getName();

	//link shaders together
	auto linkedShader = CreateRef<VulkanLinkedShader>(name, shaderLibrary.get(vertexShaderName), shaderLibrary.get(fragmentShaderName));

	add(name, linkedShader);
	return;
}

Ref<VulkanLinkedShader> VulkanLinkedShaderLibrary::get(const std::string& name) const
{
	PSIM_ASSERT(exists(name), "Shader not found!");
	return m_LinkedShaders[name];
}

bool VulkanLinkedShaderLibrary::exists(const std::string& name) const
{
	return m_LinkedShaders.find(name) != m_LinkedShaders.end();
}