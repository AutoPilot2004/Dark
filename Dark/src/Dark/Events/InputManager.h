#pragma once
#include <array>
#include <glm/glm.hpp>

#include "Utils/KeyCodes.h"
#include "Utils/MouseButtonCodes.h"

typedef struct GLFWwindow GLFWwindow;

namespace Dark
{
	class Window;

	class InputManager
	{
	public:
		// Call this function after the creation of the window and before any other function of this class
		static void init(Window* window);

		static bool isDown(int keyOrButton);
		static bool isPressed(int keyOrButton);
		static bool isReleased(int keyOrButton);

		//Getters
		static inline const glm::dvec2& getMousePosition() { return m_mousePosition; }

	private:
		InputManager() = delete;

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

		static void windowSizeCallback(GLFWwindow* window, int width, int height);

	private:
		static Window* m_window;

		static glm::dvec2 m_mousePosition;

		static std::array<bool, DARK_KEY_LAST + 1> m_keysAndButtons;
	};
}