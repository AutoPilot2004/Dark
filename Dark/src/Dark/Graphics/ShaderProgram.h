#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>

namespace Dark
{
	class ShaderProgram
	{
	public:
		void create(const std::string& vertShaderPath, const std::string& fragShaderPath);

		void setUniform1iv(const std::string& uniformName, GLsizei count, const GLint* value);
		void setUniformMatrix4fv(const std::string& uniformName, GLsizei count, GLboolean transpose, const GLfloat* value);

		void bind() const;
		void unbind() const;

		//Getters
		inline GLuint getID() const { return m_programID; }

	private:
		void compileShaders(const std::string& vertShaderPath, const std::string& fragShaderPath);
		void compileShader(const std::string& shaderFilePath, GLuint& id, GLenum type);

		void linkShaders();

		GLint getUniformLocation(const std::string& uniformName);

	private:
		GLuint m_vertID = 0, m_fragID = 0;

		GLuint m_programID = 0;

		std::unordered_map<std::string, GLint> m_uniformMap;
	};
}