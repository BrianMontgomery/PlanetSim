/*#pragma once

#include "PSIM/Graphics/Asset/Shader.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

class VulkanShader : public Shader
{
public:
	VulkanShader(const std::string& filepath);
	VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~VulkanShader();

	void deleteShader();
	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetInt(const std::string& name, int value) override { return; }
	virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override { return; }
	virtual void SetFloat(const std::string& name, float value) override { return; }
	virtual void SetFloat3(const std::string& name, const glm::vec3& value) override { return; }
	virtual void SetFloat4(const std::string& name, const glm::vec4& value) override { return; }
	virtual void SetMat4(const std::string& name, const glm::mat4& value) override { return; }

	virtual const std::string& GetName() const override { return m_Name; }

	void UploadUniformInt(const std::string& name, int value) { return; }
	void UploadUniformIntArray(const std::string& name, int* values, uint32_t count) { return; }

	void UploadUniformFloat(const std::string& name, float value) { return; }
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value) { return; }
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value) { return; }
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value) { return; }

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) { return; }
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) { return; }

private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<vk::ShaderStageFlags, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<vk::ShaderStageFlags, std::string>& shaderSources);

	std::string VulkanShader::PreprocessShader(const std::string_view &fileName, const std::string &source, shaderc_shader_kind shaderKind);
	std::string CompileToAssembly(const std::string_view &fileName, const std::string &source, shaderc_shader_kind shaderKind);
	std::vector<uint32_t> CompileFileToSPV(const std::string_view &fileName, const std::string &source, shaderc_shader_kind shaderKind);

private:
	// Helper classes taken from glslc: https://github.com/google/shaderc
	class FileFinder
	{
	public:
		std::string FindReadableFilepath(const std::string &filename) const;
		std::string FindRelativeReadableFilepath(const std::string &requesting_file, const std::string &filename) const;
		std::vector<std::string> &search_path() { return search_path_; }

	private:
		std::vector<std::string> search_path_;
	};

	class FileIncluder : public shaderc::CompileOptions::IncluderInterface
	{
	public:
		explicit FileIncluder(const FileFinder *file_finder) : file_finder_(*file_finder) {}

		~FileIncluder() override;
		shaderc_include_result *GetInclude(const char *requested_source,
			shaderc_include_type type,
			const char *requesting_source,
			size_t include_depth) override;
		void ReleaseInclude(shaderc_include_result *include_result) override;
		const std::unordered_set<std::string> &file_path_trace() const { return included_files_; }

	private:
		const FileFinder &file_finder_;
		struct FileInfo
		{
			const std::string full_path;
			std::vector<char> contents;
		};
		std::unordered_set<std::string> included_files_;
	};

private:
	std::string m_Name;
	shaderc::Compiler m_Compiler;
	shaderc::CompileOptions m_CompileOptions;
	FileFinder m_Finder{};
	VulkanFrameWork* framework;

	vk::ShaderModule vertexShader;
	vk::ShaderModule fragmentShader;
};*/