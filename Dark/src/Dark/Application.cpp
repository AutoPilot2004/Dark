#include "Application.h"
#include "Events/InputManager.h"
#include "Scene/Scene.h"
#include "Renderer/Renderer2D.h"
#include "Utils/Dark.h"
#include "Scene/SceneManager.h"
#include "GUI/GUI.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

namespace Dark
{
	void Application::run()
	{
		onInit();
		SceneManager::onInitScenes();

		gameLoop();

		onDestroy();
		Renderer2D::destroy();
		ImGui::onDestroy();
		SceneManager::onDestroyScenes();
	}

	void Application::setWindowTitle(const std::string& title)
	{
		m_window->setWindowTitle(title);
	}

	void Application::setWindowSize(const glm::ivec2& size)
	{
		m_window->setWindowSize(size);
	}

	void Application::setBackgroundColor(const Color& color)
	{
		m_window->setBackgroundColor(color);
	}

	void Application::setVSync(bool on)
	{
		m_window->setVSync(on);
	}

	void Application::startWindowMaximized()
	{
		m_window->startWindowMaximized();
	}

	void Application::init()
	{
		//Create window
		m_window = std::make_unique<Window>();
		m_window->create(1920, 1080, "Dark", { 255, 255, 255, 255 });

		//Create shader program
		m_shader = std::make_unique<ShaderProgram>();
		m_shader->create("resources/shaders/vert.vert.txt", "resources/shaders/frag.frag.txt");

		//Initialize systems
		InputManager::init(m_window.get());
		Renderer2D::init(*m_shader);
		ImGui::onInit(m_window->get());
	}

	void Application::gameLoop()
	{
		float time = glfwGetTime();

		while (!m_window->isClosed()) {
			float lastTime = time;
			time = glfwGetTime();
			float dT = time - lastTime;

			//printf("FPS: %f\n", 1.0f / dT);

			m_window->clear();

			onUpdate(dT);

			if (m_activeScene == m_prevScene) {
				m_sceneChanged = false;
			}
			else {
				m_sceneChanged = true;
				m_prevScene = m_activeScene;
			}

			if (m_activeScene) {
				if (m_activeScene->getFBO()) {
					m_activeScene->getFBO()->bind();
					m_window->clear();
				}

				m_activeScene->onUpdate(dT);

				if (m_activeScene->camChanged() || m_sceneChanged || m_window->isSizeDirty()) {
					m_activeScene->getActiveCamera()->resize(m_window->getWidth(), m_window->getHeight());

					if (m_activeScene->getFBO()) {
						m_activeScene->getFBO()->resize(m_window->getWidth(), m_window->getHeight());
					}
				}

				m_shader->bind();
				m_shader->setUniformMatrix4fv("P", 1, DARK_FALSE, glm::value_ptr(m_activeScene->getActiveCamera()->getCameraMatrix()));

				m_activeScene->onRender();

				if (m_activeScene->getFBO())
					m_activeScene->getFBO()->unbind();
			}

			m_window->swapBuffers();
			m_window->update();
		}
	}
}