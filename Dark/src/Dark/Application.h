#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Graphics/Window.h"
#include "Graphics/ShaderProgram.h"

namespace Dark
{
	class Scene;

	class Application
	{
	public:
		Application() : m_activeScene(nullptr) { init(); }
		virtual ~Application() = default;

		virtual void onInit() = 0;
		virtual void onDestroy() = 0;
		virtual void onUpdate(float dT) = 0;

		void run();

		void setWindowTitle(const std::string& title);
		void setWindowSize(const glm::ivec2& size);
		void setBackgroundColor(const Color& color);
		void setVSync(bool);

		void startWindowMaximized();

	protected:
		Scene* m_activeScene;

	private:
		void init();

		void gameLoop();

	private:
		std::unique_ptr<Window> m_window;

		//This in scenes or even entity components in the future
		std::unique_ptr<ShaderProgram> m_shader;

		Scene* m_prevScene;
		bool m_sceneChanged;
	};
}