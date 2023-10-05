#include "Window.h"
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace Dark
{
	Window::Window()
		: m_window(nullptr), m_width(0), m_height(0), m_isSizeDirty(true)
	{
		//empty
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	void Window::create(int width, int height, const std::string& title, const Color& backgroundColor)
	{
		if (!glfwInit()) {
			error("Couldn't initalize GLFW!", DARK_FILE, DARK_LINE);
		}

		m_width = width;
		m_height = height;
		m_title = title;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

		if (!m_window) {
			error("Couldn't create GLFW Window!", DARK_FILE, DARK_LINE);
		}

		glfwMakeContextCurrent(m_window);

		glfwSwapInterval(true);

		if (glewInit() != GLEW_OK) {
			error("Couldn't initalize GLEW!", DARK_FILE, DARK_LINE);
		}

		GLEC(glEnable(GL_BLEND));
		GLEC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLEC(glEnable(GL_MULTISAMPLE));

		GLEC(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));

	}

	void Window::setWindowTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_window, title.c_str());
	}

	void Window::setWindowSize(const glm::ivec2& size)
	{
		glfwSetWindowSize(m_window, m_width = size.x, m_height = size.y);

		GLEC(glViewport(0, 0, size.x, size.y));

		m_isSizeDirty = true;
	}

	void Window::setBackgroundColor(const Color& color)
	{
		GLEC(glClearColor(color.r, color.g, color.b, color.a));
	}

	void Window::setVSync(bool on)
	{
		glfwSwapInterval(on);
	}

	void Window::startWindowMaximized()
	{
		glfwMaximizeWindow(m_window);

		glfwGetWindowSize(m_window, &m_width, &m_height);
	}

	void Window::clear()
	{
		GLEC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void Window::update()
	{
		glfwPollEvents();
	}

	void Window::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

	int Window::isClosed()
	{
		return glfwWindowShouldClose(m_window);
	}
}