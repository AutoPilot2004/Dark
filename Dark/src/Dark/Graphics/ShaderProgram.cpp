#include "ShaderProgram.h"
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

#include <fstream>
#include <vector>

namespace Dark
{
	void ShaderProgram::create(const std::string& vertShaderFilePath, const std::string& fragShaderFilePath)
	{
		compileShaders(vertShaderFilePath, fragShaderFilePath);
		linkShaders();
	}

	void ShaderProgram::compileShaders(const std::string& vertShaderFilePath, const std::string& fragShaderFilePath)
	{
		compileShader(vertShaderFilePath, m_vertID, GL_VERTEX_SHADER);
		compileShader(fragShaderFilePath, m_fragID, GL_FRAGMENT_SHADER);
	}

	void ShaderProgram::compileShader(const std::string& shaderFilePath, GLuint& id, GLenum type)
	{
		GLEC(id = glCreateShader(type));

		if (id == 0) {
			std::string shaderType = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
			error("Couldn't create " + shaderType + " shader!", DARK_FILE, DARK_LINE);
		}

		std::ifstream shaderFile(shaderFilePath);

		if (shaderFile.fail()) {
			error("Couldn't open \"" + shaderFilePath + "\" file!", DARK_FILE, DARK_LINE);
		}

		std::string line;
		std::string fileContents;

		while (std::getline(shaderFile, line)) {
			fileContents += line + "\n";
		}

		const GLchar* fileContPtr = &fileContents[0];
		GLEC(glShaderSource(id, 1, &fileContPtr, nullptr));

		GLEC(glCompileShader(id));

		GLint isCompiled = 0;
		GLEC(glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			GLEC(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> infoLog(maxLength);
			GLEC(glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]));

			GLEC(glDeleteShader(id));

			error(infoLog.data(), DARK_FILE, DARK_LINE);
		}
	}

	void ShaderProgram::linkShaders()
	{
		GLEC(m_programID = glCreateProgram());

		GLEC(glAttachShader(m_programID, m_vertID));
		GLEC(glAttachShader(m_programID, m_fragID));

		GLEC(glLinkProgram(m_programID));

		GLint isLinked = 0;
		GLEC(glGetProgramiv(m_programID, GL_LINK_STATUS, (int *)&isLinked));
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			GLEC(glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> infoLog(maxLength);
			GLEC(glGetProgramInfoLog(m_programID, maxLength, &maxLength, &infoLog[0]));

			GLEC(glDeleteProgram(m_programID));

			GLEC(glDeleteShader(m_vertID));
			GLEC(glDeleteShader(m_fragID));

			error(infoLog.data(), DARK_FILE, DARK_LINE);
		}

		GLEC(glDetachShader(m_programID, m_vertID));
		GLEC(glDetachShader(m_programID, m_fragID));

		GLEC(glDeleteShader(m_vertID));
		GLEC(glDeleteShader(m_fragID));
	}

	void ShaderProgram::setUniform1iv(const std::string& uniformName, GLsizei count, const GLint* value)
	{
		GLEC(glUniform1iv(getUniformLocation(uniformName), count, value));
	}

	void ShaderProgram::setUniformMatrix4fv(const std::string& uniformName, GLsizei count, GLboolean transpose, const GLfloat* value)
	{
		GLEC(glUniformMatrix4fv(getUniformLocation(uniformName), count, transpose, value));
	}

	GLint ShaderProgram::getUniformLocation(const std::string& uniformName)
	{
		if (m_uniformMap.find(uniformName) == m_uniformMap.end())
			GLEC(m_uniformMap[uniformName] = glGetUniformLocation(m_programID, uniformName.c_str()));

		return m_uniformMap.at(uniformName);
	}


	void ShaderProgram::bind() const
	{
		GLEC(glUseProgram(m_programID));
	}

	void ShaderProgram::unbind() const
	{
		GLEC(glUseProgram(0));
	}
}