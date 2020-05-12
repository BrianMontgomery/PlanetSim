#pragma once

#include "PSIM/Graphics/Shader.h"
#include <glm/glm.hpp>

#include <glad/glad.h>

// TODO: REMOVE!
typedef unsigned int GLenum;

class OpenGLShader
{
public:
	OpenGLShader(const std::string& ShaderPath);
	~OpenGLShader();

	GLenum init(GLuint program);
	GLenum compile(GLuint program, const std::string& extension);

	const std::string& getName() const { return m_Name; }
	const std::string getType() const;

	std::string readFile();

private:
	const std::string extractExtension();
	const std::string extractName();
	const GLenum extractType();
private:
	std::string m_Path;
	std::string m_Name;
	GLenum m_Type;
};

class OpenGLShaderLibrary
{
public:
	void add(const std::string& name, const Ref<OpenGLShader>& shader);

	Ref<OpenGLShader> load(const std::string& filepath);
	Ref<OpenGLShader> load(const std::string& name, const std::string& filepath);

	Ref<OpenGLShader> get(const std::string& name);
	bool exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<OpenGLShader>> m_Shaders;
};

class OpenGLLinkedShader : public LinkedShader
{
public:
	OpenGLLinkedShader(const Ref<OpenGLShader> vertexShader, const Ref<OpenGLShader> fragmentShader);
	OpenGLLinkedShader(const std::string& name, const Ref<OpenGLShader> vertexShader, const Ref<OpenGLShader> fragmentShader);
	virtual ~OpenGLLinkedShader() override;

	void link(GLuint program, std::array<GLenum, 2> glShaderIDs);

	virtual void bind() override;
	virtual void unbind() override;

	virtual const std::string& getName() const override { return m_Name; }

	virtual void setInt(const std::string& name, int value) override;
	virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
	virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
	virtual void setMat4(const std::string& name, const glm::mat4& value) override;

	void uploadUniformInt(const std::string& name, int value);

	void uploadUniformFloat(const std::string& name, float value);
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
	uint32_t m_RendererID;
	std::string m_Name;
};

class OpenGLLinkedShaderLibrary : public ShaderLibrary
{
public:
	void add(const std::string& name, const Ref<OpenGLLinkedShader>& linkedShader);

	virtual Ref<LinkedShader> load(const std::string& vertexPath, const std::string& fragmentPath) override;
	virtual Ref<LinkedShader> load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) override;

	virtual Ref<LinkedShader> get(const std::string& name) override;
	virtual bool exists(const std::string& name) const override;
private:
	std::unordered_map<std::string, Ref<OpenGLLinkedShader>> m_LinkedShaders;
};