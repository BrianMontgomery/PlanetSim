#include "PSIMPCH.h"
#include "VulkanShader.h"

static std::vector<char> readFileByteCode(const std::string& filename)
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

VulkanShader::VulkanShader()
{
}


VulkanShader::~VulkanShader()
{
}

vk::ShaderModule VulkanShader::createShaderModule(const std::string& filename, vk::Device& device)
{
	PSIM_PROFILE_FUNCTION();
	const std::vector<char>& code = readFileByteCode(filename);
	//shader module  info
	vk::ShaderModuleCreateInfo createInfo = { {}, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };

	//create it
	vk::ShaderModule shaderModule;
	PSIM_ASSERT(device.createShaderModule(&createInfo, nullptr, &shaderModule) == vk::Result::eSuccess, "Failed to create shader module!");

	return shaderModule;
}