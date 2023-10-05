#include "InputManager.h"
#include "Graphics/Window.h"
#include "ECheck/ECheck.h"

#include <iostream>
#include <GLFW/glfw3.h>

namespace Dark
{
	Window* InputManager::m_window = nullptr;

	glm::dvec2 InputManager::m_mousePosition(0.0f, 0.0f);

	std::array<bool, DARK_KEY_LAST + 1> InputManager::m_keysAndButtons = { false };

	void InputManager::init(Window* window)
	{
		m_window = window;

		glfwSetKeyCallback(m_window->get(), keyCallback);
		glfwSetMouseButtonCallback(m_window->get(), mouseButtonCallback);
		glfwSetCursorPosCallback(m_window->get(), cursorPositionCallback);
		glfwSetWindowSizeCallback(m_window->get(), windowSizeCallback);
		//glfwSetFramebufferSizeCallback(m_window->getWindow(), frameBufferSizeCallback);
	}

	void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == DARK_PRESSED)
			m_keysAndButtons[key] = true;
		else if (action == DARK_RELEASED)
			m_keysAndButtons[key] = false;
	}

	void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == DARK_PRESSED)
			m_keysAndButtons[button] = true;
		else if (action == DARK_RELEASED)
			m_keysAndButtons[button] = false;
	}

	void InputManager::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		m_mousePosition = { xpos, ypos };
	}

	void InputManager::windowSizeCallback(GLFWwindow* window, int width, int height)
	{
		m_window->setWindowSize({ width, height });	
	}


	bool InputManager::isDown(int keyOrButton)
	{
		return m_keysAndButtons[keyOrButton];
	}

	bool InputManager::isPressed(int keyOrButton)
	{
		static bool alreadyPressed[DARK_KEY_LAST + 1] = { false };

		bool isPressed = (!alreadyPressed[keyOrButton] && isDown(keyOrButton));

		alreadyPressed[keyOrButton] = isDown(keyOrButton);

		return isPressed;
	}

	bool InputManager::isReleased(int keyOrButton)
	{
		static bool wasDown[DARK_KEY_LAST + 1] = { false };

		bool isReleased = (wasDown[keyOrButton] && !isDown(keyOrButton));

		wasDown[keyOrButton] = isDown(keyOrButton);

		return isReleased;
	}
}