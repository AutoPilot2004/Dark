#include "SceneManager.h"
#include "Scene.h"

namespace Dark
{
	std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::m_sceneMap;

	Scene* SceneManager::m_sceneInQueue = nullptr;

	void SceneManager::addScene(const std::string& sceneName, std::unique_ptr<Scene> scene)
	{
		m_sceneMap[sceneName] = std::move(scene);
	}

	void SceneManager::setScene(const std::string& sceneName)
	{
		m_sceneInQueue = (m_sceneMap.at(sceneName)).get();
	}

	void SceneManager::onInitScenes()
	{
		for (const auto& scene : m_sceneMap) {
			scene.second->onInit();
		}
	}

	void SceneManager::onDestroyScenes()
	{
		for (const auto& scene : m_sceneMap) {
			scene.second->onDestroy();
		}
	}
}