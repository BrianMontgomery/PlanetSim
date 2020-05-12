#include "PSIMPCH.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>


OpenGLShader::OpenGLShader(const std::string& shaderPath)
	: m_Path(shaderPath)
{
	PSIM_PROFILE_FUNCTION();

	const std::string extension = extractExtension();

	if (extension == ".spv")
	{
		m_Name = extractName();
		m_Type = extractType();
	}
	else if (extension == ".vert") {
		m_Name = extractName();
		m_Type = GL_VERTEX_SHADER;
	}
	else if (extension == ".frag") {
		m_Name = extractName();
		m_Type = GL_FRAGMENT_SHADER;
	}
	else {
		PSIM_ASSERT(false, "File extension not supported. \nSupported Extensions: .spv, .frag, .vert")
	}
}

OpenGLShader::~OpenGLShader()
{
	PSIM_PROFILE_FUNCTION();
}

//TODO used in the shaderlinker program
GLenum OpenGLShader::init(GLuint program)
{
	PSIM_PROFILE_FUNCTION();

	const std::string extension = extractExtension();

	GLenum shaderID;

	if (extension == ".spv" || extension == ".vert" || extension == ".glsl") {
		shaderID = compile(program, extension);
	}
	else {
		PSIM_ASSERT(false, "File extension not supported. \nSupported Extensions: .spv, .frag, .vert");
	}

	return shaderID;
}

//TODO put in shader to be used by linker through shaders
GLenum OpenGLShader::compile(GLuint program, const std::string& extension)
{
	PSIM_PROFILE_FUNCTION();

	GLenum glShaderID = 0;
	GLint isCompiled = 0;

	const std::string& source = readFile();

	GLuint shader = glCreateShader(m_Type);

	if (extension == ".spv")
	{
		// Apply the vertex shader SPIR-V to the shader object.
		glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), source.size());

		// Specialize the vertex shader.
		std::string vsEntrypoint = "main";
		glSpecializeShader(shader, (const GLchar*)vsEntrypoint.c_str(), 0, nullptr, nullptr);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	}
	else {
		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	}

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shader);

		PSIM_CORE_ERROR("{0}", infoLog.data());
		PSIM_ASSERT(false, "Shader compilation failure!");
	}

	glAttachShader(program, shader);

	return shader;
}

const std::string OpenGLShader::getType() const 
{
	if (m_Type == GL_VERTEX_SHADER)
	{
		const std::string type = "vertex";
		return type;
	}
	if (m_Type == GL_FRAGMENT_SHADER)
	{
		const std::string type = "fragment";
		return type;
	}
	
	PSIM_ASSERT(false, "Type not found!");
	return nullptr;
}

std::string OpenGLShader::readFile()
{
	PSIM_PROFILE_FUNCTION();

	std::string result;
	std::ifstream in(m_Path, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
			in.close();
		}
		else
		{
			PSIM_CORE_ERROR("Could not read from file '{0}'", m_Path);
		}
	}
	else
	{
		PSIM_CORE_ERROR("Could not open file '{0}'", m_Path);
	}

	return result;
}

const std::string OpenGLShader::extractExtension()
{
	PSIM_PROFILE_FUNCTION();

	std::string result;
	
	size_t extensionStart = m_Path.find_last_of('.');
	std::cout << result << std::endl;

	return result;
}

const std::string OpenGLShader::extractName()
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
	else if(extension == ".frag") {
		std::string name = m_Path.substr(lastSlash, count);
		return (name + "Frag");
	}

	return nullptr;
}

const GLenum OpenGLShader::extractType()
{
	//extract name from filepath
	auto lastDot = m_Path.rfind('.');
	auto type = (m_Path.substr(lastDot - 4, 4));

	if (type == "vert" || type == "Vert")
	{
		return GL_VERTEX_SHADER;
	}
	else if (type == "frag" || type == "Frag"){
		return GL_FRAGMENT_SHADER;
	}

	PSIM_ASSERT(false, "Type not found!");
	return GL_FALSE;
}

//---------------------------------------------------------------------------------------------------------------------------------------


//Shader Library
//---------------------------------------------------------------------------------------------------------------------------------------

void OpenGLShaderLibrary::add(const std::string& name, const Ref<OpenGLShader>& shader)
{
	PSIM_ASSERT(!exists(name), "This Shader: {0} already exists!", name);
	m_Shaders[name] = shader;
}

Ref<OpenGLShader> OpenGLShaderLibrary::load(const std::string& filepath)
{
	PSIM_PROFILE_FUNCTION();

	auto shader = CreateRef<OpenGLShader>(filepath);
	add(shader->getName(), shader);
	return shader;
}

Ref<OpenGLShader> OpenGLShaderLibrary::load(const std::string& name, const std::string& filepath)
{
	PSIM_PROFILE_FUNCTION();

	auto shader = CreateRef<OpenGLShader>(filepath);
	add(name, shader);
	return shader;
}

Ref<OpenGLShader> OpenGLShaderLibrary::get(const std::string& name)
{
	PSIM_ASSERT(exists(name), "Shader not found!");
	return m_Shaders[name];
}


bool OpenGLShaderLibrary::exists(const std::string& name) const
{
	return m_Shaders.find(name) != m_Shaders.end();
}

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shaders
//---------------------------------------------------------------------------------------------------------------------------------------

OpenGLLinkedShader::OpenGLLinkedShader(const Ref<OpenGLShader> vertexShader, const Ref<OpenGLShader> fragmentShader)
{
	GLuint program = glCreateProgram();
	std::array<GLenum, 2> glShaderIDs;

	glShaderIDs[0] = vertexShader->init(program);
	glShaderIDs[1] = fragmentShader->init(program);

	m_RendererID = program;

	link(program, glShaderIDs);

	m_Name = vertexShader->getName().substr(0, (vertexShader->getName().size() - 4));
}

OpenGLLinkedShader::OpenGLLinkedShader(const std::string& name, const Ref<OpenGLShader> vertexShader, const Ref<OpenGLShader> fragmentShader)
	: m_Name(name)
{
	GLuint program = glCreateProgram();
	std::array<GLenum, 2> glShaderIDs;

	glShaderIDs[0] = vertexShader->init(program);
	glShaderIDs[1] = fragmentShader->init(program);

	m_RendererID = program;

	link(program, glShaderIDs);
}

OpenGLLinkedShader::~OpenGLLinkedShader()
{
	PSIM_PROFILE_FUNCTION();

	glDeleteProgram(m_RendererID);
}

//TODO put in shader linker
void OpenGLLinkedShader::link(GLuint program, std::array<GLenum, 2> glShaderIDs)
{
	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);

		for (auto id : glShaderIDs)
			glDeleteShader(id);

		PSIM_CORE_ERROR("{0}", infoLog.data());
		PSIM_ASSERT(false, "Shader link failure!");
		return;
	}

	for (auto id : glShaderIDs)
	{
		glDetachShader(program, id);
		glDeleteShader(id);
	}
}

void OpenGLLinkedShader::bind()
{
	PSIM_PROFILE_FUNCTION();

	glUseProgram(m_RendererID);
}

void OpenGLLinkedShader::unbind()
{
	PSIM_PROFILE_FUNCTION();

	glUseProgram(0);
}

void OpenGLLinkedShader::setInt(const std::string& name, int value)
{
	PSIM_PROFILE_FUNCTION();

	uploadUniformInt(name, value);
}

void OpenGLLinkedShader::setFloat3(const std::string& name, const glm::vec3& value)
{
	PSIM_PROFILE_FUNCTION();

	uploadUniformFloat3(name, value);
}

void OpenGLLinkedShader::setFloat4(const std::string& name, const glm::vec4& value)
{
	PSIM_PROFILE_FUNCTION();

	uploadUniformFloat4(name, value);
}

void OpenGLLinkedShader::setMat4(const std::string& name, const glm::mat4& value)
{
	PSIM_PROFILE_FUNCTION();

	uploadUniformMat4(name, value);
}

void OpenGLLinkedShader::uploadUniformInt(const std::string& name, int value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1i(location, value);
}

void OpenGLLinkedShader::uploadUniformFloat(const std::string& name, float value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1f(location, value);
}

void OpenGLLinkedShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void OpenGLLinkedShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLLinkedShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLLinkedShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLLinkedShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

//---------------------------------------------------------------------------------------------------------------------------------------


//Linked Shaders Library
//---------------------------------------------------------------------------------------------------------------------------------------

void OpenGLLinkedShaderLibrary::add(const std::string& name, const Ref<OpenGLLinkedShader>& linkedShader)
{
	PSIM_ASSERT(!exists(name), "This Linked Shader: {0} group already exists!", name);
	m_LinkedShaders[name] = linkedShader;
}

Ref<LinkedShader> OpenGLLinkedShaderLibrary::load(const std::string& vertexPath, const std::string& fragmentPath)
{
	PSIM_PROFILE_FUNCTION();

	OpenGLShaderLibrary shaderLibrary;
	//load vertex shader
	auto vertexShader = shaderLibrary.load(vertexPath);
	auto vertexShaderName = vertexShader->getName();

	//load fragment shader
	auto fragmentShader = shaderLibrary.load(fragmentPath);
	auto fragmentShaderName = fragmentShader->getName();

	//link shaders together
	auto linkedShader = CreateRef<OpenGLLinkedShader>(shaderLibrary.get(vertexShaderName), shaderLibrary.get(fragmentShaderName));

	//add to the linked shader list
	add(linkedShader->getName(), linkedShader);
	return linkedShader;
}

Ref<LinkedShader> OpenGLLinkedShaderLibrary::load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
	PSIM_PROFILE_FUNCTION();

	//load vertex shader
	OpenGLShaderLibrary shaderLibrary;
	auto vertexShader = shaderLibrary.load(vertexPath);
	auto vertexShaderName = vertexShader->getName();

	//load fragment shader
	auto fragmentShader = shaderLibrary.load(fragmentPath);
	auto fragmentShaderName = fragmentShader->getName();

	//link shaders together
	auto linkedShader = CreateRef<OpenGLLinkedShader>(name, shaderLibrary.get(vertexShaderName), shaderLibrary.get(fragmentShaderName));

	add(name, linkedShader);
	return linkedShader;
}

Ref<LinkedShader> OpenGLLinkedShaderLibrary::get(const std::string& name)
{
	PSIM_ASSERT(exists(name), "Shader not found!");
	return m_LinkedShaders[name];
}

bool OpenGLLinkedShaderLibrary::exists(const std::string& name) const
{
	return m_LinkedShaders.find(name) != m_LinkedShaders.end();
}