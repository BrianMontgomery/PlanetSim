#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class LinkedShader
{
public:
	virtual ~LinkedShader() = default;

	virtual void bind() = 0;
	virtual void unbind() = 0;

	virtual void setInt(const std::string& name, int value) = 0;
	virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
	virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
	virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

	virtual const std::string& getName() const = 0;
};

/*
Usage:
	Load a shader usign load.
	Retrieve the shader from the library and bind it in a pipeline.

	The shaders need to be bound in a pipeline in order tobe used.

*/
class ShaderLibrary
{
public:
	virtual void load(const std::string& vertexPath, const std::string& fragmentPath) = 0;
	virtual void load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) = 0;

	virtual bool exists(const std::string& name) const = 0;

	static Ref<ShaderLibrary> ShaderLibrary::Create();
};