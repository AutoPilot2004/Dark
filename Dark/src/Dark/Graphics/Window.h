#pragma once
#include <string>

#include "Vertex.h"

typedef struct GLFWwindow GLFWwindow;

namespace Dark
{
	class Window
	{
	public:
		Window();
		~Window();

		void create(int width, int height, const std::string& title, const Color& backgroundColor);

		void setWindowTitle(const std::string& title);
		void setWindowSize(const glm::ivec2& size);

		//TODO: CHANGE THIS, MAYBE PUT IN THE "GRAPHICS" CPP FILE AND MAKE "GRAPHICS" CLASS/NAMESPACE
		static void setBackgroundColor(const Color& color);
		void setVSync(bool);

		void startWindowMaximized();

		void clear();
		void update();
		void swapBuffers();

		int isClosed();

		//Getters
		inline int getWidth() const { return m_width; }
		inline int getHeight() const { return m_height; }

		inline bool isSizeDirty() { bool dirty = m_isSizeDirty; if (dirty) m_isSizeDirty = false; return dirty; }

	private:
		inline GLFWwindow* get() const { return m_window; }

	private:
		GLFWwindow* m_window;

		int m_width, m_height;
		std::string m_title;

		bool m_isSizeDirty;

		friend class Application;
		friend class InputManager;
	};
}