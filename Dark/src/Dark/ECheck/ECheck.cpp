#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void clearGLErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool displayGLErrors(const char* functionName, const char* fileName, int line)
{
	bool hasNoError = true;

	while (GLenum error = glGetError()) {
		std::cout << "The function \"" << functionName << "\" in file \"" << fileName << "\" has an error, " << error << "! (Line: " << line << ")" << std::endl;
		hasNoError = false;
	}

	return hasNoError;
}

namespace Dark
{
	void error(const std::string& errorMessage, const char* file, int line)
	{
		glfwTerminate();
		printf("%s (File: \"%s\", Line: %d)\n", errorMessage.c_str(), file, line);
		std::cin.get();
		std::exit(1);
	}
}