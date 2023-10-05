#include "Application.h"
#include "EditorScene.h"

#include <Scene/SceneManager.h>
#include <fstream>

void Application::onInit()
{
	printf("App Initialized!\n");

	Dark::SceneManager::addScene("Editor Scene", std::make_unique<EditorScene>());
	Dark::SceneManager::setScene("Editor Scene");
}

void Application::onDestroy()
{
	printf("App Destroyed!\n");
}

void Application::onUpdate(float dT)
{
	if (Dark::SceneManager::sceneInQueue()) {
		if (m_activeScene)
			m_activeScene->onExit();
		m_activeScene = Dark::SceneManager::getAndResetSceneInQueue();
		m_activeScene->onEntry();
	}
}